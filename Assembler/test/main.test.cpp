#include "Assembler/AssemblerPipe.h"
#include "Lexer/LexerPipe.h"
#include "Parser/ParserPipe.h"

#include "rasm/read.h"
#include "rasm/write.h"

int main()
{
    using namespace RaychelScript::Pipes; //NOLINT

    Logger::setMinimumLogLevel(Logger::LogLevel::debug);
    {

        const auto data_or_error = Lex{lex_file, "../../../shared/test/simple.rsc"} | Parse{} | Assemble{};

        if (log_if_error(data_or_error)) {
            return 1;
        }

        {
            const auto data = data_or_error.value();
            if (!RaychelScript::Assembly::write_rsbf("./simple.rsbf", data)) {
                return 1;
            }
        }
    }

    {
        const auto data_or_error = RaychelScript::Assembly::read_rsbf("./simple.rsbf");
        if (const auto* ec = std::get_if<RaychelScript::Assembly::ReadingErrorCode>(&data_or_error); ec) {
            Logger::error(*ec, '\n');
            return 1;
        }

        const auto data = Raychel::get<RaychelScript::Assembly::VMData>(data_or_error);
        for (const auto& [value, address] : data.immediate_values) {
            Logger::info("\t$", static_cast<std::uint32_t>(address.value()), " -> ", value, '\n');
        }

        std::size_t index{0};
        for (const auto instr : data.instructions) {
            Logger::log(index++, ": ", instr, '\n');
        }
    }
}