#include <Defines.hpp>
#include <core/utils/Result.hpp>
#include <core/storage/String.hpp>

namespace pe::core::io {

enum class ErrorSAX {
    INTERNAL,
    _COUNT
};

using ResultSAX = utils::Result<void, ErrorSAX>;

class IProtocolSAX : public BaseObject<>
{
    virtual ResultSAX key(std::string_view name) = 0;
    virtual ResultSAX metakey(std::string_view name) = 0;

    virtual ResultSAX null() = 0;

    virtual ResultSAX boolean(bool value) = 0;

    virtual ResultSAX number(int32_t value) = 0;
    virtual ResultSAX number(uint32_t value) = 0;
    virtual ResultSAX number(int64_t value) = 0;
    virtual ResultSAX number(uint64_t value) = 0;
    virtual ResultSAX number(float value) = 0;
    virtual ResultSAX number(double value) = 0;

    virtual ResultSAX text(std::string_view text) = 0;
    virtual ResultSAX text(storage::String&& text) = 0;

    virtual ResultSAX binaryStream(const uint8_t* bytes, size_t size) = 0;

    virtual ResultSAX startObject() = 0;
    virtual ResultSAX endObject() = 0;

    virtual ResultSAX startArray() = 0;
    virtual ResultSAX endArray() = 0;
};

class IEventHandlerSAX : IProtocolSAX
{
    virtual uint8_t* binaryStreamBufferRequest(size_t size) { return nullptr; }
    virtual void binaryStreamBufferFilled(const ResultSAX& status) {}
};

class IReaderSAX
{
    virtual ResultSAX read(IEventHandlerSAX& handler);
};

class IWriterSAX : IProtocolSAX
{
    virtual ResultSAX write();
};

} // namespace pe::core::io