/** @file colt_args.h
* Contains Command Line argument parsing helpers.
* To parse the arguments, call `ParseArguments(argc, argv)`.
* This will populate the global argument holder.
* The colt::args namespace holds the GlobalArguments reference which points to the parsed
* arguments. In case ParseArguments was not called, it contained the defaulted values of
* arguments.
*/

#ifndef HG_COLT_ARGS
#define HG_COLT_ARGS

#include <array>
#include <algorithm>
#include <filesystem>

#include <colt/String.h>
#include <code_gen/opt_level.h>

/// @brief Contains utilities for parsing command line arguments
namespace colt::args
{
	/// @brief Holds the result of parsing command line arguments
	struct Arguments
	{
		/// @brief The path to the file to compile
		const char* file_in = nullptr;
		/// @brief The path where to output the compiled file
		const char* file_out = nullptr;
		/// @brief If true, allows printing functions to use colored output
		bool colored_output = true;
		/// @brief Print resulting LLVM IR
		bool print_llvm_ir = false;
		/// @brief If true, allows messages to be printed
		bool print_messages = true;
		/// @brief If true, allows warnings to be printed
		bool print_warnings = true;
		/// @brief If true, allows errors to be printed
		bool print_errors = true;
		/// @brief If true, wait for user input before exiting
		bool wait_for_user_input = true;
		/// @brief If true, the compiler will attempt to run the 'main' function if it exists
		bool jit_run_main = false;
		/// @brief Optimization level
		gen::OptimizationLevel opt_level = static_cast<gen::OptimizationLevel>(0);
	};

	/// @brief Parses the command line arguments, and stores them globally.
	/// This functions exits if the arguments are not valid.
	/// @param argc The argument count
	/// @param argv The argument values
	void ParseArguments(int argc, const char** argv) noexcept;

	namespace details
	{
		/// @brief Global arguments that can be accessed by args::GlobalArguments
		inline Arguments global_args;

		/// @brief Specifies the arguments of an Argument callback
		using ARG_CALLBACK = void(*)(int, const char**, size_t&) noexcept;

		/// @brief Represents a command line argument
		struct Argument
		{
			/// @brief --name
			StringView name;
			/// @brief -abrv
			StringView abrv;
			/// @brief The -h [--name/-abrv]
			StringView help;
			/// @brief The number of expected arguments
			size_t nb_of_args;
			/// @brief The function to call when the Argument found was valid
			ARG_CALLBACK callback;
		};

		void version_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void help_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void enum_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void print_ir_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void no_color_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void no_error_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void no_warning_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void no_message_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void out_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void no_wait_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void o0_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void o1_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void o2_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void o3_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void os_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void oz_callback(int argc, const char** argv, size_t& current_arg) noexcept;
		void run_main_callback(int argc, const char** argv, size_t& current_arg) noexcept;

		/// @brief Contains all predefined valid arguments
		constexpr std::array PredefinedArguments
		{
			Argument{ "version", "v", "Prints the version of the compiler.\nUse: --version/-v", 0, &version_callback},
			Argument{ "help", "h", "Prints the documentation of a command.\nUse: --help/-h <COMMAND>", 1, &help_callback},
			Argument{ "enum", "e", "Enumerates all possible commands.\nUse: --enum/-e", 0, &enum_callback},
			Argument{ "print-ir", "i", "Prints generated LLVM IR.\nUse: --print-ir/-i", 0, &print_ir_callback},
			Argument{ "no-color", "C", "Removes colored/highlighted outputs on the console.\nUse: --no-color/-C", 0, &no_color_callback},
			Argument{ "no-error", "E", "Removes error outputs.\nUse: --no-error/-E", 0, &no_error_callback},
			Argument{ "no-warn", "W", "Removes warning outputs.\nUse: --no-warn/-W", 0, &no_warning_callback},
			Argument{ "no-message", "M", "Removes message outputs.\nUse: --no-message/-M", 0, &no_message_callback},
			Argument{ "out", "o", "Specifies the output location.\nUse: --out/-o <PATH>", 1, &out_callback},
			Argument{ "no-wait", "NW", "Specifies that the compiler should exit without user input.\nUse: --no-wait/-W", 0, &no_wait_callback},
			Argument{ "opt-0", "O0", "Disable most optimizations.\nUse: --opt-0/-O0", 0, &o0_callback},
			Argument{ "opt-1", "O1", "Optimize quickly without hindering debuggability.\nUse: --opt-1/-O1", 0, &o1_callback},
			Argument{ "opt-2", "O2", "Optimize for fast execution as much as possible without significantly increasing compile times.\nUse: --opt-2/-O2", 0, &o2_callback},
			Argument{ "opt-3", "O3", "Optimize for fast execution as much as possible.\nUse: --opt-3/-O3", 0, &o3_callback},
			Argument{ "opt-s", "Os", "Optimize for small code size instead of fast execution.\nUse: --opt-s/-Os", 0, &os_callback},
			Argument{ "opt-z", "Oz", "Optimize for small code size at all cost.\nUse: --opt-z/-Oz", 0, &oz_callback},
			Argument{ "run-main", "r", "Run 'main' function inside the compiler if it exists.\nUse: --run-main/-r", 0, &run_main_callback},
		};

		/// @brief Handles an argument, searching for it and doing error handling
		/// @param arg_view The current argument to parse
		/// @param argc The argument count
		/// @param argv The argument values
		/// @param current_arg The number of the current argument
		void handle_arg(StringView arg_view, int argc, const char** argv, size_t& current_arg) noexcept;

		/// @brief Searches for an argument in predefined_args
		/// @param arg_view The argument to find
		/// @return Iterator to the argument or predefined_args.end() if not found
		auto find_arg_in_predefined(StringView arg_view) noexcept -> decltype(PredefinedArguments.end());		
	}

	/// @brief Global Arguments used throughout the Colt-Compiler
	inline const Arguments& GlobalArguments = details::global_args;

	namespace details
	{
		template<typename... Args>
		/// @brief Prints an error and exits
		[[noreturn]] void print_error_and_exit(fmt::format_string<Args...> fmt, Args&&... args) noexcept;

		template<typename... Args>
		void print_error_and_exit(fmt::format_string<Args...> fmt, Args && ...args) noexcept
		{
			if (args::GlobalArguments.colored_output)
				fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "Error: ");
			else
				fmt::print("Error: ");

			fmt::print(fmt, std::forward<Args>(args)...);
			std::fputc('\n', stdout);
			std::exit(1);
		}
	}
}

#endif //!HG_COLT_ARGS