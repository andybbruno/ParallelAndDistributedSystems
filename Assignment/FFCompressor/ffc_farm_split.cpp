/*
 * Simple file compressor using miniz and the FastFlow pipeline.
 *
 * miniz source code: https://github.com/richgel999/miniz
 * https://code.google.com/archive/p/miniz/
 * 
 */
/* Author: Massimo Torquati <massimo.torquati@unipi.it>
 * This code is a mix of POSIX C code and some C++ library call 
 * (mainly for strings manipulation).
 */

#include <miniz.h>

#include <string>
#include <iostream>
#include <filesystem>
#include <ff/ff.hpp>
#include <ff/pipeline.hpp>
#include <utility.hpp>
#include <map>
#include "lib/utimer.cpp"
using namespace ff;

struct Task
{
	Task(const std::vector<unsigned char> &bytes, size_t size, const std::string &name, const std::string &oname, size_t nch = 1) : bytes(bytes),
																																	size(size),
																																	nchunks(nch),
																																	filename(name),
																																	original_filename(oname) {}

	std::vector<unsigned char> bytes;
	size_t size;
	size_t nchunks;
	const std::string filename;
	const std::string original_filename;
};

// 1st stage
struct Read : ff_node_t<Task>
{
	const char **argv;
	const int argc;
	bool success = true;
	size_t threshold;

	Read(const char **argv, int argc, size_t threshold) : argv(argv), argc(argc), threshold(threshold) {}

	// ------------------- utility functions
	// It memory maps the input file and then assigns a task to
	// one Worker
	bool doWork(const std::string &fname)
	{
		auto infile = std::ifstream(fname, std::ios::out | std::ios::binary);
		auto fsize = std::filesystem::file_size(fname);

		unsigned char *ptr = nullptr;

		if (fsize > threshold)
		{
			//SPLIT THE FILE
			//SEND TO THE WORKERS CHUNKS OF DATA
			int nchunks = fsize / threshold;

			size_t lastindex = fname.find_last_of(".");
			std::string rawname = fname.substr(0, lastindex);

			for (int i = 0; i < nchunks; i++)
			{
				int sz = (i == (nchunks - 1)) ? fsize % threshold : threshold;
				std::vector<unsigned char> data(sz);
				for (int i = 0; i < sz; i++)
				{
					infile.read((char *)&data[i], sizeof(unsigned char));
				}
				std::string partname(rawname + ".part" + std::to_string(i));
				// std::cout << "nchunks:" << nchunks << std::endl;
				Task *t = new Task(data, sz, partname, fname, nchunks);
				ff_send_out(t);

				// std::cout << "i: " << i << "\t\tsize: " << sz << "\t\tname: " << partname << std::endl;
				// int position = i * threshold;
				// unsigned char *ptr = &data[position];
				// int sz = (i == (nchunks - 1)) ? fsize % threshold : threshold;
				// Task *t = new Task(ptr, sz, (fname + std::to_string(i)));
				// ff_send_out(t);
			}
		}
		else
		{
			//SEND PLAIN DATA
			std::vector<unsigned char> data(fsize);
			for (int i = 0; i < fsize; i++)
			{
				infile.read((char *)&data[i], sizeof(unsigned char));
			}
			Task *t = new Task(data, fsize, fname, fname);
			ff_send_out(t); // sending to the next stage
		}
		return true;
	}
	// walks through the directory tree rooted in dname
	bool walkDir(const std::string &dname, size_t size)
	{
		DIR *dir;
		if ((dir = opendir(dname.c_str())) == NULL)
		{
			perror("opendir");
			fprintf(stderr, "Error: opendir %s\n", dname.c_str());
			return false;
		}
		struct dirent *file;
		bool error = false;
		while ((errno = 0, file = readdir(dir)) != NULL)
		{
			struct stat statbuf;
			std::string filename = dname + "/" + file->d_name;
			if (stat(filename.c_str(), &statbuf) == -1)
			{
				perror("stat");
				fprintf(stderr, "Error: stat %s\n", filename.c_str());
				return false;
			}
			if (S_ISDIR(statbuf.st_mode))
			{
				if (!isdot(filename.c_str()))
				{
					if (!walkDir(filename, statbuf.st_size))
						error = true;
				}
			}
			else
			{
				if (!doWork(filename))
					error = true;
			}
		}
		if (errno != 0)
		{
			perror("readdir");
			error = true;
		}
		closedir(dir);
		return !error;
	}
	// -------------------

	Task *svc(Task *)
	{
		for (long i = 0; i < argc; ++i)
		{
			struct stat statbuf;
			if (stat(argv[i], &statbuf) == -1)
			{
				perror("stat");
				fprintf(stderr, "Error: stat %s\n", argv[i]);
				continue;
			}
			bool dir = false;
			if (S_ISDIR(statbuf.st_mode))
			{
				success &= walkDir(argv[i], statbuf.st_size);
			}
			else
			{
				success &= doWork(argv[i]);
			}
		}
		return EOS;
	}

	void svc_end()
	{
		if (!success)
		{
			printf("Read stage: Exiting with (some) Error(s)\n");
			return;
		}
	}
};
// 2nd stage
struct Compressor : ff_node_t<Task>
{
	Task *svc(Task *task)
	{
		unsigned char *inPtr = reinterpret_cast<unsigned char *>(task->bytes.data());

		size_t inSize = task->size;
		// get an estimation of the maximum compression size
		unsigned long cmp_len = compressBound(inSize);
		// allocate memory to store compressed data in memory
		unsigned char *ptrOut = new unsigned char[cmp_len];
		if (compress(ptrOut, &cmp_len, (const unsigned char *)inPtr, inSize) != Z_OK)
		{
			printf("Failed to compress file in memory\n");
			success = false;
			delete[] ptrOut;
			return GO_ON;
		}

		task->bytes = std::vector<unsigned char>(ptrOut, ptrOut + cmp_len);
		// task->cmp_size = cmp_len;
		ff_send_out(task);

		// unmapFile(inPtr, inSize);
		return GO_ON;
	}
	void svc_end()
	{
		if (!success)
		{
			printf("Comprssor stage: Exiting with (some) Error(s)\n");
			return;
		}
	}
	bool success = true;
};
// 3rd stage
struct Write : ff_node_t<Task>
{
	Task *svc(Task *task)
	{
		const std::string outfile = task->filename + ".zip";
		// write the compressed data into disk

		unsigned char *inPtr = reinterpret_cast<unsigned char *>(task->bytes.data());

		success &= writeFile(outfile, inPtr, task->bytes.size());
		if (success && REMOVE_ORIGIN)
		{
			unlink(task->filename.c_str());
		}
		// delete task;
		ff_send_out(task);
		return GO_ON;
	}
	void svc_end()
	{
		if (!success)
		{
			printf("Write stage: Exiting with (some) Error(s)\n");
			return;
		}
	}
	bool success = true;
};

// 4th stage
struct FileMerge : ff_node_t<Task>
{
	std::map<std::string, int> mapping;

	Task *svc(Task *task)
	{
		auto it = mapping.find(task->original_filename);
		if (it == mapping.end())
		{
			mapping.insert(std::pair<std::string, int>(task->original_filename, task->nchunks - 1));
		}
		else
		{
			it->second -= 1;
			// std::cout << it->first << std::endl;
			// std::cout << it->second << std::endl;
			if (it->second == 0)
			{
				size_t lastindex = task->original_filename.find_last_of(".");
				std::string rawname = task->original_filename.substr(0, lastindex);

				std::string stemp("tar -cf " + rawname + ".zip " + rawname + ".part*.zip");
				// std::cout << stemp << std::endl;
				system(stemp.c_str());

				// std::string delstr("find . -name \"" + rawname + ".part*.zip\" -delete");
				// std::cout << delstr << std::endl;
				// system(delstr.c_str());
			}
		}
		delete task;
		return GO_ON;
	}
};

static inline void
usage(const char *argv0)
{
	printf("--------------------\n");
	printf("Usage: NW %s file-or-directory\n", argv0);
	printf("\nModes: COMPRESS ONLY\n");
	printf("--------------------\n");
}

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		usage(argv[0]);
		return -1;
	}
	argc--;

	int nw = atoi(argv[1]);
	size_t threshold = atoll(argv[3]);

	Read reader(const_cast<const char **>(&argv[2]), 1, threshold);
	Compressor compress;
	Write writer;
	FileMerge fmerge;

	ff_Pipe<Task> pipe(compress, writer);

	std::vector<std::unique_ptr<ff_node>> W;
	for (size_t i = 0; i < nw; ++i)
	{
		// std::unique_ptr<ff_node_t<Task>> rdr(new Read());
		std::unique_ptr<ff_node_t<Task>> com(new Compressor());
		std::unique_ptr<ff_node_t<Task>> wrt(new Write());

		W.push_back(std::unique_ptr<ff_Pipe<Task>>(new ff_Pipe<Task>(com, wrt)));
	}

	ff_Farm<Task> farm(std::move(W), reader, fmerge);

	// farm.remove_collector();
	//farm.wrap_around();

	std::cout << nw;
	utimer u("");

	if (farm.run_and_wait_end() < 0)
	{
		error("running farm");
		return -1;
	}

	bool success = true;
	success &= reader.success;
	success &= compress.success;
	success &= writer.success;

	system("find . -name \"*.part*.zip\" -delete");
	return 0;
}
