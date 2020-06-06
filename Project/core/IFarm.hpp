/**
 * @author Andrea Bruno
 * @brief FARM INTERFACE
 * @date 06-2020
 */

/**
 * @brief Emitter Interface
 */
template <typename OUT>
class IEmitter
{
public:
	/**
	 * @brief Check if there is something to emit
	 */
	virtual bool hasNext() = 0;

	/**
	 * @brief Create(emit) a new instance of type OUT
	 */
	virtual OUT next() = 0;
};

/**
 * @brief Worker Interface
 */
template <typename IN, typename OUT>
class IWorker
{
public:
	/**
	 * @brief The logic of each Worker
	 */
	virtual OUT compute(IN &x) = 0;
};

/**
 * @brief Collector Interface
 */
template <typename IN, typename OUT>
class ICollector
{
public:
	/**
	 * @brief The logic of the Collector
	 */
	virtual OUT collect(IN const &x) = 0;
};
