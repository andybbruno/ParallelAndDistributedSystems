// stream source
template <typename OUT>
class ISource
{
public:
    virtual OUT next() = 0;
    virtual bool hasNext() = 0;
};

// business logic to compute a task
template <typename IN, typename OUT>
class IWorker
{
public:
    virtual OUT compute(IN x) = 0;
};

// processing the results: accumulate the stream contents
template<typename IN>
class IDrain {
public:
  virtual void process(IN x) = 0;
};
