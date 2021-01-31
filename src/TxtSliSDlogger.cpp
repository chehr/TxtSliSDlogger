/*!
This code logging data from fischertechnik ROBOPro to the TXT SD Card.

The source code is based from SLI examples established by CvanLeeuwen,
see https://github.com/fischertechnik/txt_demo_ROBOPro_SLI

spdlog see https://github.com/gabime/spdlog, Copyright(c) 2015 Gabi Melman.

TXT SLI SD Card Logger Version 1.0 (proof of concept)
TXT Data Logger to SD-Card Copyright(c) 30.01.2021 Christian Hehr
*/
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <stdio.h>

#include "KeLibTxtDl.h"          // TXT Lib
#include "FtShmem.h"             // TXT Transfer Area


#include <iostream>       // std::cout
#include <cstring>
#include <string>

// Includes for the spdlog
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/async.h"


//static is optional, the variable will be created by the runtime when it loads the SLI.
//Remark: The SLi is not unloaded when a RoboPro program has been terminated/stop.
static bool IsInit = false;      /*!< Set after initialization*/
static bool IsStop = false;      /*!< Set after stop */

static double value_d=0.0; 	/*!< Example of a global double variable */
static short value_s=0; 	/*!< Example of a global short variable */



/*!
  \brief spdlog globals, two loggers.
*/
static std::shared_ptr<spdlog::logger> console = nullptr;      /*!< spdlogger */
//spdlog::rotating_logger_mt<spdlog::async_factory>("console", "Data/SliCon.log", 248576 * 1, 8,true);
static std::shared_ptr<spdlog::logger> file_logger = nullptr; /*!< spdlogger */
//spdlog::rotating_logger_mt<spdlog::async_factory>("file_logger", "Data/SliLogr.log", 248576 * 1, 8,true);




extern "C" {
/*!
 * @brief Can be used to initialize this SLI. <br/>
 *          It is also necessary to check if the SLI has been stopped.
 * @remark The TXT load a SLI lib on the first time that it used.
 * After ending a RoboPro program, the SLI stays in memory. The global values keep their last values.
 * @param[out] t
 * @return  0: success,continuation by the workflow
 *          Other values error, continuation by the error workflow
 */
/**********************************************************************
 * init
***********************************************************************/

/*!
 * @param *t input, not in use
 * @return  0: success, otherwise error ()
 */
int init(short *t) {
	if (IsStop) {
		//Expected;
		printf("*****init: IsStop\n");
		IsStop=false;
	} else {
		printf("*****init: Not expected, was not IsStop\n");
		value_s = 2;   // Error 2 Init failed
		//Execute the stop
	}
	if (!IsInit) {
		//Todo: Init what is necessary
		IsInit = true;
		printf("*****init: Set has been done\n");
	} else {
		printf("*****init: Not expected, was already done ");
		//What is the meaning of this?
	}
	return 0;
}
/**********************************************************************
 * SpdLog related
***********************************************************************/

/*!
 *Init both spd loggers<br/>
 * console and file_logger
 * @see  <a href="https://github.com/gabime/spdlog/blob/v1.x/README.md">SPDLog info</a>
 */
int  getInitSpdLogShort(short *t) {
	std::cout << "############Start init  " << std::endl;
	try {
		if (spdlog::get("file_logger") == nullptr) {
			file_logger = spdlog::rotating_logger_mt("file_logger",
					"/opt/knobloch/ExternalSD/TXT/Daten.log", 1048576 * 1024, 6, true);  // max logging size (1024 MB), max logging files (3)
			std::cout << "############file_logger init " << std::endl;
		} else{
			SPDLOG_LOGGER_INFO(file_logger, "flush ");
			std::cout << "############file_logger flush"	<< std::endl;
			file_logger->flush();
		}

		if (spdlog::get("console") == nullptr) {
			std::cout << "############Not init " << std::endl;
			console = spdlog::stdout_color_mt("console");
			std::cout << "############console init " << std::endl;
		} else{
			SPDLOG_LOGGER_INFO(console, "flush ");
			std::cout << "############console flush "	<< std::endl;
			console ->flush();
		}
		spdlog::set_pattern("[%t] [%Y-%m-%d %T.%f %i] %v ");
		// spdlog::set_pattern("[thread %t][%Y-%m-%d %T.%e][%L](%^%@%$ %!) %v");
		//spdlog::get("console")->set_pattern("####[%t %T.%e][%L][%^%@:%!%$] %v");
		spdlog::set_level(spdlog::level::trace);

	} catch (const spdlog::spdlog_ex &ex) {
		std::cout << "############init:Log initialization failed: " << ex.what()
				<< std::endl;
		value_s = 3; // Error code 3, No SD-Card included
		return -2;
	}
	SPDLOG_LOGGER_INFO(file_logger, "[Thread id] [Date and time microsec Elapsed time in microsec ] RoboPro floating value.");
	SPDLOG_LOGGER_INFO(console, "Initialization has been finished.");

	*t = (short) 25;
	return 0;
}


int getFlushDropSpdLogShort(short *t) {
	if (file_logger != nullptr) {
		SPDLOG_LOGGER_INFO(file_logger, "Flush.");
		file_logger->flush();
		std::cout << "############ flush file_logger " << std::endl;
		value_s = 1; // No error, stopped by user
	} else
		std::cout << "############ no file_logger " << std::endl;
	if (console != nullptr) {
		SPDLOG_LOGGER_INFO(console, "Ende.");
		value_s = 1; // No error, stopped by user
		std::cout << "############ flush  console " << std::endl;
	} else
		std::cout << "############ no console " << std::endl;
	if (file_logger != nullptr
			|| console != nullptr) {
		spdlog::drop_all();
		std::cout << "############ drop all " << std::endl;
	}
	*t = (short) 25;
	return 0;
}



/*****************************************************************************************
* the SLI functions.
*****************************************************************************************/
/*!
 * @brief set a global double value in the SLI
 * @param[in] v the value for value_d
 * @return  0: success,continuation by the workflow
 *          Other values error, continuation by the error workflow
 */
int setValueDouble(double data_FP48) {
	if (!IsInit) {
		SPDLOG_LOGGER_TRACE(file_logger,"FP48 -> SD-save not initialized" );
		value_s = 2; // Error code 2, Init failed
		return -1;
	}
	SPDLOG_LOGGER_TRACE(file_logger, data_FP48);  // Daten auf SD-Karte loggen
	return 0;
}
/*!
 * @brief set a global short value in the SLI
 * @param[in] v the value for value_s
 * @return  0: success,continuation by the workflow
 *          Other values error, continuation by the error workflow
 */
int setValueShort(short data_Int16) {
	if (!IsInit) {
		SPDLOG_LOGGER_TRACE(file_logger,"Init16 -> SD-save not initialized" );
		value_s = 2; // Error code 2, Init failed
		return -1;
	}
	SPDLOG_LOGGER_TRACE(file_logger,data_Int16);  // Daten auf SD-Karte loggen
	return 0;
}

/*!
 * @brief get a global double value from the SLI
 * @param[out] v the value of value_d
 * @return  0: success,continuation by the workflow
 *          Other values error, continuation by the error workflow
 */
int getValueDouble(double *v) {
	 if (!IsInit) {
		fprintf(stderr, "ExampleSLI:getValueDouble: Not initialized!\n");
		return -1;
	}
	*v = value_d;
	printf("****ExampleSLI:getValueDouble: %f\n", *v);
	return 0;
}
/*!
 * @brief get a global short value from the SLI
 * @param[out] v the value of value_s
 * @return  0: success,continuation by the workflow
 *          Other values error, continuation by the error workflow
 */
int getValueShort(short *v) {
	if (!IsInit) {
		fprintf(stderr, "ExampleSLI:getValueShort: Not initialized!\n");
		value_s = 2;  // Init failed
		return -1;
	} else
	value_s = 1;  // Stopped by user
	*v = value_s;
	printf(" SD-Card logging stopped by user: %d\n", *v);
	return 0;
}

} // extern "C"
