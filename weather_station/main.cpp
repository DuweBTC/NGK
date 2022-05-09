#include <iostream>

#include <restinio/all.hpp>
#include <restinio/websocket/websocket.hpp>
#include <json_dto/pub.hpp>

struct place_t
{
	place_t() = default;
	place_t(
		std::string placeName,
		double lat,
		double lon)
		: m_placeName{std::move(placeName)}, m_lat{std::move(lat)}, m_lon{std::move(lon)}
	{
	}

	template <typename JSON_IO>
	void
	json_io(JSON_IO &io)
	{
		io
				&json_dto::mandatory("placeName", m_placeName) &
			json_dto::mandatory("lat", m_lat) & json_dto::mandatory("lon", m_lon);
	}

	std::string m_placeName;
	double m_lat;
	double m_lon;
};

struct weather_t
{
	weather_t() = default;

	weather_t(
		std::string id,
		std::string date,
		std::string time,
		place_t place,
		double temperature,
		int64_t humidity)
		: m_id{std::move(id)}, m_date{std::move(date)}, m_time{std::move(time)}, m_place{std::move(place)}, m_temperature{std::move(temperature)}, m_humidity{std::move(humidity)}
	{
	}

	template <typename JSON_IO>
	void
	json_io(JSON_IO &io)
	{
		io
				&json_dto::mandatory("id", m_id) &
			json_dto::mandatory("date", m_date) & json_dto::mandatory("time", m_time) & json_dto::mandatory("place", m_place) & json_dto::mandatory("temperature", m_temperature) & json_dto::mandatory("humidity", m_humidity);
	}

	std::string m_id;
	std::string m_date;
	std::string m_time;
	place_t m_place;
	double m_temperature;
	int64_t m_humidity;
};

using weather_struct = std::vector<weather_t>;

namespace rr = restinio::router;
using router_t = rr::express_router_t<>;

namespace rws = restinio::websocket::basic;

using traits_t =
			restinio::traits_t<
				restinio::asio_timer_manager_t,
				restinio::single_threaded_ostream_logger_t,
				router_t>;
// Alias for container with stored websocket handles.
using ws_registry_t = std::map< std::uint64_t, rws::ws_handle_t >;

class weather_handler_t
{
public:
	explicit weather_handler_t(weather_struct &weather)
		: m_weather(weather)
	{
	}

	weather_handler_t(const weather_handler_t &) = delete;
	weather_handler_t(weather_handler_t &&) = delete;

	auto on_weather_list(
		const restinio::request_handle_t &req, rr::route_params_t) const
	{
		auto resp = init_resp(req->create_response());

		resp.set_body(
			"Weather Station (days count: " +
			std::to_string(m_weather.size()) + ")\n");

		for (std::size_t i = 0; i < m_weather.size(); ++i)
		{
			resp.append_body(std::to_string(i + 1) + ". ");
			const auto &b = m_weather[i];
			resp.append_body("ID: " + b.m_id + "\n Time: " + b.m_date + "  " + b.m_time + "\n"
																						  "Place: " +
							 b.m_place.m_placeName + " " +
							 std::to_string(b.m_place.m_lat) + " " + std::to_string(b.m_place.m_lon) + "\n Temperature: " + std::to_string(b.m_temperature) +
							 "\n Humidity: " + std::to_string(b.m_humidity) + "% \n "

			);
		}

		return resp.done();
	}

	auto on_weather_get(
		const restinio::request_handle_t &req, rr::route_params_t params)
	{
		const auto weathernum = restinio::cast_to<std::uint32_t>(params["weathernum"]);

		auto resp = init_resp(req->create_response());

		if (0 != weathernum && weathernum <= m_weather.size())
		{
			const auto &b = m_weather[weathernum - 1];
			resp.set_body(
				"Weather #" + std::to_string(weathernum) + " is: " + b.m_id + "\n Time: " + b.m_date + "  " + b.m_time + "\n"
																														 "Place: " +
				b.m_place.m_placeName + " " +
				std::to_string(b.m_place.m_lat) + " " + std::to_string(b.m_place.m_lon) + "\n Temperature: " + std::to_string(b.m_temperature) +
				"\n Humidity: " + std::to_string(b.m_humidity) + "% \n ");
		}
		else
		{
			resp.set_body(
				"No weather with #" + std::to_string(weathernum) + "\n");
		}

		return resp.done();
	}
	/*
	//Implementation of id
	auto on_id_get(
		const restinio::request_handle_t &req, rr::route_params_t params)
	{
		auto resp = init_resp(req->create_response());
		try
		{
			auto id = restinio::utils::unescape_percent_encoding(params["id"]);

			resp.set_body("Weather of " + id + ":\n");

			for (std::size_t i = 0; i < m_weather.size(); ++i)
			{
				const auto &b = m_weather[i];
				if (id == b.m_id)
				{
					resp.append_body(std::to_string(i + 1) + ". ");
					resp.append_body("ID: " + b.m_id + "\n Time: " + b.m_date + "  " + b.m_time + "\n"
																								  "Place: " +
									 b.m_place.m_placeName + " " +
									 std::to_string(b.m_place.m_lat) + " " + std::to_string(b.m_place.m_lon) + "\n Temperature: " + std::to_string(b.m_temperature) +
									 "\n Humidity: " + std::to_string(b.m_humidity) + "% \n "

					);
				}
			}
		}
		catch (const std::exception &)
		{
			mark_as_bad_request(resp);
		}

		return resp.done();
	}
	*/

	// Implemtation of get date
	auto on_list_get(
		const restinio::request_handle_t &req, rr::route_params_t params)
	{
		auto resp = init_resp(req->create_response());
		try
		{

			resp.set_body("Weather on date for the last three:\n");

			for (std::size_t i = m_weather.size() - 3; i < m_weather.size(); ++i)
			{
				if (i < 0)
					i = 0;

				const auto &b = m_weather[i];

				resp.append_body(std::to_string(i + 1) + ". ");
				resp.append_body("ID: " + b.m_id + "\n Time: " + b.m_date + "  " + b.m_time + "\n"
																							  "Place: " +
								 b.m_place.m_placeName + " " +
								 std::to_string(b.m_place.m_lat) + " " + std::to_string(b.m_place.m_lon) + "\n Temperature: " + std::to_string(b.m_temperature) +
								 "\n Humidity: " + std::to_string(b.m_humidity) + "% \n "

				);
			}
		}
		catch (const std::exception &)
		{
			mark_as_bad_request(resp);
		}

		return resp.done();
	}

	// Implemtation of get date
	auto on_date_get(
		const restinio::request_handle_t &req, rr::route_params_t params)
	{
		auto resp = init_resp(req->create_response());
		try
		{
			auto date = restinio::utils::unescape_percent_encoding(params["date"]);

			resp.set_body("Weather on date " + date + ":\n");

			for (std::size_t i = 0; i < m_weather.size(); ++i)
			{
				const auto &b = m_weather[i];
				if (date == b.m_date)
				{
					resp.append_body(std::to_string(i + 1) + ". ");
					resp.append_body("ID: " + b.m_id + "\n Time: " + b.m_date + "  " + b.m_time + "\n"
																								  "Place: " +
									 b.m_place.m_placeName + " " +
									 std::to_string(b.m_place.m_lat) + " " + std::to_string(b.m_place.m_lon) + "\n Temperature: " + std::to_string(b.m_temperature) +
									 "\n Humidity: " + std::to_string(b.m_humidity) + "% \n "

					);
				}
			}
		}
		catch (const std::exception &)
		{
			mark_as_bad_request(resp);
		}

		return resp.done();
	}

	auto on_new_weather(
		const restinio::request_handle_t &req, rr::route_params_t)
	{
		auto resp = init_resp(req->create_response());

		try
		{
			m_weather.emplace_back(
				json_dto::from_json<weather_t>(req->body()));
		}
		catch (const std::exception & /*ex*/)
		{
			mark_as_bad_request(resp);
		}

		return resp.done();
	}

	auto on_weather_update(
		const restinio::request_handle_t &req, rr::route_params_t params)
	{
		const auto weathernum = restinio::cast_to<std::uint32_t>(params["weathernum"]);

		auto resp = init_resp(req->create_response());

		try
		{
			auto b = json_dto::from_json<weather_t>(req->body());

			if (0 != weathernum && weathernum <= m_weather.size())
			{
				m_weather[weathernum - 1] = b;
			}
			else
			{
				mark_as_bad_request(resp);
				resp.set_body("No weather with #" + std::to_string(weathernum) + "\n");
			}
		}
		catch (const std::exception & /*ex*/)
		{
			mark_as_bad_request(resp);
		}

		return resp.done();
	}

	auto on_weather_delete(
		const restinio::request_handle_t &req, rr::route_params_t params)
	{
		const auto weathernum = restinio::cast_to<std::uint32_t>(params["weathernum"]);

		auto resp = init_resp(req->create_response());

		if (0 != weathernum && weathernum <= m_weather.size())
		{
			const auto &b = m_weather[weathernum - 1];
			resp.set_body(
				"Delete weather #" + std::to_string(weathernum) + ": " +
				b.m_date + "[" + b.m_id + "]\n");

			m_weather.erase(m_weather.begin() + (weathernum - 1));
		}
		else
		{
			resp.set_body(
				"No weather with #" + std::to_string(weathernum) + "\n");
		}

		return resp.done();
	}

	auto on_live_update(const restinio::request_handle_t& req, rr::route_params_t params)
	{
		if (restinio::http_connection_header_t::upgrade == req->header().connection())
		{
			auto wsh = rws::upgrade<traits_t>(*req, rws::activation_t::immediate,[this](auto wsh, auto m)
			{
				if( rws::opcode_t::text_frame == m->opcode() ||	rws::opcode_t::binary_frame == m->opcode() ||rws::opcode_t::continuation_frame == m->opcode() )
				{
					wsh->send_message( *m );
				}
					else if( rws::opcode_t::ping_frame == m->opcode() )
				{
					auto resp = *m;
					resp.set_opcode( rws::opcode_t::pong_frame );
					wsh->send_message( resp );
				}
				else if( rws::opcode_t::connection_close_frame == m->opcode() )
				{
					register->erase( wsh->connection_id() );
				}

			});
		m_registry.emplace(wsh->connection_id(), wsh);

		init_resp(req->create_response()).done();

		return restinio::request_accepted();

		}

	return restinio::request_rejected();
	}

private:
	weather_struct &m_weather;

	template <typename RESP>
	static RESP
	init_resp(RESP resp)
	{
		resp
			.append_header("Server", "RESTinio sample server /v.0.6")
			.append_header_date_field()
			.append_header("Content-Type", "text/plain; charset=utf-8");

		return resp;
	}

	template <typename RESP>
	static void
	mark_as_bad_request(RESP &resp)
	{
		resp.header().status_line(restinio::status_bad_request());
	}
};

auto server_handler(weather_struct &weather_data)
{
	auto router = std::make_unique<router_t>();
	auto handler = std::make_shared<weather_handler_t>(std::ref(weather_data));

	auto by = [&](auto method)
	{
		using namespace std::placeholders;
		return std::bind(method, handler, _1, _2);
	};

	auto method_not_allowed = [](const auto &req, auto)
	{
		return req->create_response(restinio::status_method_not_allowed())
			.connection_close()
			.done();
	};

	// Handlers for '/' path.
	router->http_get("/", by(&weather_handler_t::on_weather_list));
	router->http_post("/", by(&weather_handler_t::on_new_weather));
	router->http_get("/chat", by(&weather_handler_t::on_live_update));

	// Disable all other methods for '/'.
	router->add_handler(
		restinio::router::none_of_methods(
			restinio::http_method_get(), restinio::http_method_post()),
		"/", method_not_allowed);

	// Handler for '/date/:date' path.
	router->http_get("/date/:date", by(&weather_handler_t::on_date_get));

	// Disable all other methods for '/date/:date'.
	router->add_handler(
		restinio::router::none_of_methods(restinio::http_method_get()),
		"/date/:date", method_not_allowed);

	// Handler for '/list/' path.
	router->http_get("/list/", by(&weather_handler_t::on_list_get));

	// Disable all other methods for '/list/:list'.
	router->add_handler(
		restinio::router::none_of_methods(restinio::http_method_get()),
		"/list/", method_not_allowed);

	// Handlers for '/:weathernum' path.
	router->http_get(
		R"(/:weathernum(\d+))",
		by(&weather_handler_t::on_weather_get));
	router->http_put(
		R"(/:weathernum(\d+))",
		by(&weather_handler_t::on_weather_update));
	router->http_delete(
		R"(/:weathernum(\d+))",
		by(&weather_handler_t::on_weather_delete));

	// Disable all other methods for '/:weathernum'.
	router->add_handler(
		restinio::router::none_of_methods(
			restinio::http_method_get(),
			restinio::http_method_post(),
			restinio::http_method_delete()),
		R"(/:weathernum(\d+))", method_not_allowed);

	return router;

	
}

int main()
{
	using namespace std::chrono;

	try
	{
		using traits_t =
			restinio::traits_t<
				restinio::asio_timer_manager_t,
				restinio::single_threaded_ostream_logger_t,
				router_t>;

		weather_struct weather_data{
			{"1", "20211105", "12:15", {"Aarhus N", 13692, 19.438}, 13.1, 70},
			{"2", "20211105", "13:15", {"Aarhus N", 13692, 19.438}, 15.1, 60},
			{"3", "20211105", "14:15", {"Aarhus N", 13692, 19.438}, 14.1, 65},
		};

		
		restinio::run(
			restinio::on_this_thread<traits_t>()
				.address("localhost")
				.request_handler(server_handler(weather_data))
				.read_next_http_message_timelimit(10s)
				.write_http_response_timelimit(1s)
				.handle_request_timeout(1s));
	}
	catch (const std::exception &ex)
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
