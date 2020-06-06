/**
 * @author Andrea Bruno
 * @brief MASTER-WORKER INTERFACE
 * @date 06-2020
 */

/**
 * @brief Master Interface
 */
template <typename OUT>
class IMaster
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