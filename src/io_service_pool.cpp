#include "io_service_pool.h"
#include "xassert.h"
#include <stdexcept>
#include <boost/thread.hpp>
#include <boost/bind.hpp>


namespace global
{

	io_service_pool::io_service_pool(std::size_t pool_size)
		: step(io_service_pool::BEGIN)
		, next_io_service_(0)
	{
		// Give all the io_services work to do so that their run() functions will not
		// exit until they are explicitly stopped.
		for (std::size_t i = 0; i < pool_size; ++ i)
		{
			io_service_ptr io_service(new boost::asio::io_service);
			work_ptr work(new boost::asio::io_service::work(*io_service));
			io_services_.push_back(io_service);
			work_.push_back(work);
		}
	}

	void io_service_pool::start()
	{
		boost::recursive_mutex::scoped_lock lock(mtx);
		XASSERT(step == BEGIN);
		step = START;
		for (std::size_t i = 0; i < io_services_.size(); ++ i)
		{
			boost::shared_ptr<boost::thread> thread(new boost::thread(
				boost::bind(&boost::asio::io_service::run, io_services_[i])));
			threads.push_back(thread);
		}		
	}

	void io_service_pool::join()
	{
		boost::recursive_mutex::scoped_lock lock(mtx);
		if (step == STOP)
		{
			step = FINISH;
			for (std::size_t i = 0; i < threads.size(); ++ i)
			{
				threads[i]->join();
			}
		}
	}

	void io_service_pool::stop()
	{
		boost::recursive_mutex::scoped_lock lock(mtx);
		if (step == START)
		{
			step = STOP;
			// Explicitly stop all io_services.
			for (std::size_t i = 0; i < io_services_.size(); ++ i)
			{
				io_services_[i]->stop();
			}
		}
	}

	boost::asio::io_service& io_service_pool::get_io_service()
	{
		if (io_services_.size() == 0)
		{
			throw std::runtime_error("get_io_service when io_service_pool size is 0");
		}
		// Use a round-robin scheme to choose the next io_service to use.
		boost::asio::io_service& io_service = *io_services_[next_io_service_];
		++ next_io_service_;
		if (next_io_service_ == io_services_.size())
		{
			next_io_service_ = 0;
		}
		return io_service;
	}

} 
