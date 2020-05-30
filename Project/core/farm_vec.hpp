template <typename OUT>
class IEmitter
{
public:
  virtual OUT next() = 0;
  virtual bool hasNext() = 0;
};

template <typename IN, typename OUT>
class IWorker
{
public:
  virtual OUT compute(IN &x) = 0;
};

template <typename IN, typename OUT>
class ICollector
{
public:
  virtual OUT collect(IN const &x) = 0;
};
