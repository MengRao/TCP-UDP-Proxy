#pragma once

#include "type.h"
#include <vector>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace global
{
	
	class io_service_pool
		: public boost::noncopyable
	{
	public:
		/// Construct the io_service pool.
		explicit io_service_pool(std::size_t pool_size);

		/// Start all io_service objects in the pool.
		void start();

		/// join all io_service objects in the pool.
		void join();

		/// Stop all io_service objects in the pool.
		void stop();

		/// Get an io_service to use.
		boost::asio::io_service& get_io_service();

	private:
		typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
		typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

		/// The pool of io_services.
		std::vector<io_service_ptr> io_services_;

		/// The work that keeps the io_services running.
		std::vector<work_ptr> work_;

		/// The pool of threads to run all of the io_services.
		std::vector<boost::shared_ptr<boost::thread> > threads;
		boost::recursive_mutex mtx;
		enum step_t
		{
			BEGIN = 0,
			START = 1,
			STOP = 2,
			FINISH = 3,
		};
		step_t step;

		/// The next io_service to use for a connection.
		std::size_t next_io_service_;
	};


} // namespace global
