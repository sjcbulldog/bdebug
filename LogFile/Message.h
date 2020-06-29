#pragma once

#include <string>
#include <filesystem>

namespace bwg
{
	namespace logfile
	{
		class Message
		{
		public:
			enum class Type
			{
				Info,
				Debug,
				Warning,
				Error,
				Fatal
			};

		public:
			Message(Type type, const std::string &module);
			virtual ~Message();

			void clear() {
				text_.clear();
			}

			Type type() const {
				return type_;
			}

			const std::string& module() const {
				return module_;
			}

			const std::string& text() const {
				return text_;
			}

			Message& operator<<(const char* txt) {
				text_ += txt;
				return *this;
			}

			Message& operator<<(const std::string& txt) {
				text_ += txt;
				return *this;
			}

			Message& operator<<(const std::filesystem::path& path) {
				text_ += path.generic_string();
				return *this;
			}

			Message& operator<<(uint8_t v) {
				text_ += std::to_string(v);
				return *this;
			}

			Message& operator<<(uint16_t v) {
				text_ += std::to_string(v);
				return *this;
			}

			Message& operator<<(uint32_t v) {
				text_ += std::to_string(v);
				return *this;
			}

			Message& operator<<(uint64_t v) {
				text_ += std::to_string(v);
				return *this;
			}

			std::string messageString() const;

		private:
			Type type_;
			std::string module_;
			std::string text_;
		};
	}
}
