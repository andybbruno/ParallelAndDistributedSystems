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
	Task(char *bytes, size_t size, const std::string &name, const std::string &oname, size_t nch = 1) : bytes(bytes),
																										size(size),
																										nchunks(nch),
																										filename(name),
																										original_filename(oname) {}

	char *bytes;
	size_t size;
	size_t nchunks;
	const std::string filename;
	const std::string original_filename;
};

struct MiniTask
{
	MiniTask(const std::string &name, const std::string &oname, size_t nch) : nchunks(nch),
																			  filename(name),
																			  original_filename(oname) {}

	size_t nchunks;
	const std::string filename;
	const std::string original_filename;
};

struct Splitter : ff_node_t<MiniTask>
{
	bool success = true;
	size_t threshold;
	char *filefolder;

	Splitter(char *filefolder, size_t threshold) : filefolder(filefolder), threshold(threshold) {}

	bool split(const std::string &fname)
	{
		auto infile = std::ifstream(fname, std::ios::out | std::ios::binary);
		auto fsize = std::filesystem::file_size(fname);

		unsigned char *ptr = nullptr;

		//SPLIT THE FILE
		if (fsize > threshold)
		{
			int nchunks = fsize / threshold;
			for (int i = 0; i < nchunks; i++)
			{
				int sz = (i == (nchunks - 1)) ? fsize % threshold : threshold;
				char data[sz];
				infile.read(data, sz);
				std::string partname(fname + ".tmp.part" + std::to_string(i));
				auto partfile = std::ofstream(partname, std::ios::out | std::ios::binary);
				if (partfile)
				{
					partfile.write(data, sz);
					MiniTask *t = new MiniTask(partname, fname, nchunks);
					ff_send_out(t); // sending to the next stage

					// std::cout << "Split: " << partname << std::endl;
				}
				else
				{
					// std::cout << "Problema file: " << partname << std::endl;
					return false;
				}
			}
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
				if (!split(filename))
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

	MiniTask *svc(MiniTask *)
	{
		struct stat statbuf;
		if (stat(filefolder, &statbuf) == -1)
		{
			perror("stat");
			fprintf(stderr, "Error: stat %s\n", filefolder);
		}
		bool dir = false;
		if (S_ISDIR(statbuf.st_mode))
		{
			success &= walkDir(filefolder, statbuf.st_size);
		}
		else
		{
			success &= split(filefolder);
		}
		return EOS;
	}

	void svc_end()
	{
		if (!success)
		{
			printf("Split stage: Exiting with (some) Error(s)\n");
			return;
		}
	}
};

// 1st stage
struct Read : ff_node_t<MiniTask, Task>
{
	bool success = true;
	Task *svc(MiniTask *mt)
	{
		std::string fname = mt->filename;

		auto infile = std::ifstream(fname, std::ios::out | std::ios::binary);
		if (!infile)
		{
			success = false;
		}
		else
		{
			auto fsize = std::filesystem::file_size(fname);
			char data[fsize];
			infile.read(data, fsize);

			int nchunks = mt->nchunks;
			auto oname = mt->original_filename;

			if (nchunks > 1)
			{
				Task *t = new Task(data, fsize, fname, oname, nchunks);
				ff_send_out(t); // sending to the next stage
				// std::cout << "Read: " << fname << "\t" << oname << std::endl;
			}
			else
			{
				Task *t = new Task(data, fsize, fname, fname);
				ff_send_out(t); // sending to the next stage
				// std::cout << "Read: " << fname << std::endl;
			}
		}
		return GO_ON;
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
		// std::cout << "COMPRESSOR" << std::endl;
		unsigned char *inPtr = reinterpret_cast<unsigned char *>(task->bytes);

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

		task->bytes = reinterpret_cast<char *>(ptrOut);
		task->size = cmp_len;
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
		// std::cout << "WRITE" << std::endl;
		const std::string outfile = task->filename + ".zip";
		// write the compressed data into disk

		unsigned char *inPtr = reinterpret_cast<unsigned char *>(task->bytes);

		success &= writeFile(outfile, inPtr, task->size);
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
		// std::cout << "MERGE:" << task->original_filename << "\t" << task->filename << std::endl;
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
				std::string stemp("tar -cf " + task->original_filename + ".zip " + task->original_filename + ".tmp.part*.zip");
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

	Splitter split(argv[2], threshold);
	Read reader;
	Compressor compress;
	Write writer;
	FileMerge fmerge;

	ff_Pipe<Task> pipe(compress, writer);

	std::vector<std::unique_ptr<ff_node>> W;
	for (size_t i = 0; i < nw; ++i)
	{
		std::unique_ptr<ff_node_t<Task>> com(new Compressor());
		std::unique_ptr<ff_node_t<Task>> wrt(new Write());
		W.push_back(std::unique_ptr<ff_Pipe<Task>>(new ff_Pipe<Task>(com, wrt)));
	}

	ff_Farm<Task> farm(std::move(W), reader);
	farm.set_scheduling_ondemand();
	ff_Pipe<Task> complete(split, farm, fmerge);

	utimer u("");

	if (complete.run_and_wait_end() < 0)
	{
		error("running farm");
		return -1;
	}

	system("find . -name \"*.tmp.part*\" -delete");

	std::cout << nw;
	return 0;
}
