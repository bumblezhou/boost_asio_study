//
// experimental/deferred.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_EXPERIMENTAL_DEFERRED_HPP
#define BOOST_ASIO_EXPERIMENTAL_DEFERRED_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>
#include <boost/asio/deferred.hpp>

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace experimental {

#if !defined(BOOST_ASIO_NO_DEPRECATED)
using boost::asio::deferred_t;
using boost::asio::deferred;
#endif // !defined(BOOST_ASIO_NO_DEPRECATED)

} // namespace experimental
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_EXPERIMENTAL_DEFERRED_HPP
