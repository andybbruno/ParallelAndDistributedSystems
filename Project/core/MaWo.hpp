template <typename OUT>
class IMaster
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