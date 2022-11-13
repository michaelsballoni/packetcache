#include "pch.h"
#include "CppUnitTest.h"

#include "cache_list.h"
#include "utils.h"

#pragma comment(lib, "pluspacketlib.lib")

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace packetcache
{
	using byte_array = std::vector<uint8_t>;

	TEST_CLASS(pluspacktests)
	{
	public:
		TEST_METHOD(TestUtils)
		{
			std::string test_str = "foobar";
			byte_array test_buffer = str_to_buffer(test_str.c_str());

			Assert::AreEqual(test_str, buffer_to_str(test_buffer));

			auto hash1 = hash_bytes(test_buffer.data(), test_buffer.size());
			Assert::AreNotEqual(0U, hash1);
			auto hash2 = hash_bytes(test_buffer.data(), test_buffer.size());
			Assert::AreEqual(hash1, hash2);

			auto crc1 = crc_bytes(test_buffer.data(), test_buffer.size());
			Assert::AreNotEqual(0U, crc1);
			auto crc2 = crc_bytes(test_buffer.data(), test_buffer.size());
			Assert::AreEqual(crc1, crc2);
		}

		TEST_METHOD(TestCacheList)
		{
			const std::string test_str1 = "foobar";
			const byte_array test_buffer1 = str_to_buffer(test_str1.c_str());

			const std::string test_str2 = "bletmonkey";
			const byte_array test_buffer2 = str_to_buffer(test_str2.c_str());

			cache_list list;

			auto added_entry1 = list.add(0, test_buffer1);
			size_t size1 = list.size();
			Assert::AreNotEqual(size_t(0), size1);

			Assert::IsTrue(added_entry1 == list.head());
			Assert::IsTrue(added_entry1 == list.tail());

			auto gotten_buffer1 = list.get(added_entry1);
			Assert::AreEqual(test_str1, buffer_to_str(gotten_buffer1));

			auto added_entry2 = list.add(0, test_buffer2);
			size_t size2 = list.size();
			Assert::IsTrue(size2 > size1);

			Assert::IsTrue(added_entry2 == list.head());
			Assert::IsTrue(added_entry1 == list.tail());

			gotten_buffer1 = list.get(added_entry1);
			Assert::AreEqual(test_str1, buffer_to_str(gotten_buffer1));

			Assert::IsTrue(added_entry1 == list.head());
			Assert::IsTrue(added_entry2 == list.tail());

			list.remove(added_entry2);

			Assert::IsTrue(added_entry1 == list.head());
			Assert::IsTrue(added_entry1 == list.tail());

			list.clear();
			Assert::IsTrue(nullptr == list.head());
			Assert::IsTrue(nullptr == list.tail());
			Assert::AreEqual(size_t(0), list.size());
		}
	};
}
