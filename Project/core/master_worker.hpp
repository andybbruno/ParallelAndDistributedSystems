template <typename OUT>
class IMaster
{
public:
  virtual OUT next() = 0;
  virtual bool hasNext() = 0;
};

template <typename IN>
class IWorker
{
public:
  virtual void compute(IN &x) = 0;
};