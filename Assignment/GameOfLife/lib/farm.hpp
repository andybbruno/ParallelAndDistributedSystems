// stream source
template <typename OUT>
class IEmitter
{
public:
  virtual OUT next() = 0;
  virtual bool hasNext() = 0;
};

// business logic to compute a task
template <typename IN>
class IWorker
{
public:
  virtual void compute(IN x) = 0;
};