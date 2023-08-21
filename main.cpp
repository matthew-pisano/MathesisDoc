#include <iostream>
#include "src/plain_io.h"
#include "src/encodings.h"
#include "lib/CLI11/CLI11.hpp"
#include "src/io_factory.h"

void testReadWrite() {
    PlainReader reader("../data/doc.txt");
    std::string content = reader.read();
    std::cout << content << std::endl;

    PlainWriter writer("../data/doc.txt");
    writer.write(content+"\n\nWow!");

    content = reader.read();
    std::cout << content << std::endl;
}

int encodeCommand(const std::string& inFile, std::string outFile, const std::string& encoding) {
    IOFactory factory = IOFactory();

    if(outFile.empty())
        outFile = std::filesystem::path(inFile).replace_extension(encodingFromName(encoding)->ext()).string();

    Reader* reader = factory.reader(PlainEncoding().name(), inFile);
    Writer* writer = factory.writer(encoding, outFile);

    writer->write(reader->read());

    return 0;
}

int decodeCommand(const std::string& inFile, std::string outFile, const std::string& encoding) {
    IOFactory factory = IOFactory();

    if(outFile.empty())
        outFile = std::filesystem::path(inFile).replace_extension(PlainEncoding().ext()).string();

    Reader* reader = factory.reader(encoding, inFile);
    Writer* writer = factory.writer(PlainEncoding().name(), outFile);

    writer->write(reader->read());

    return 0;
}

int main(int argc, char** argv) {

    CLI::App app{"Mathesis document encoder-decoder", "MathesisDoc"};

    std::string inFile;
    std::string outFile;
    std::string encoding;

    CLI::App* encode = app.add_subcommand("encode", "Encodes a file");
    encode->add_option("-f,--file,file", inFile, "The file to encode")->required();
    encode->add_option("-o,--out-file", outFile, "The file to output the encoding to");
    encode->add_option("-e,--encoding", encoding, "The encoding to use during execution")->required();

    CLI::App* decode = app.add_subcommand("decode", "Decodes a file");
    decode->add_option("-f,--file,file", inFile, "The file to decode")->required();
    decode->add_option("-o,--out-file", outFile, "The file to output the decoding to");
    decode->add_option("-e,--encoding", encoding, "The encoding to use during execution");

    try {
        app.parse(argc, argv);

        if(encode->parsed()) return encodeCommand(inFile, outFile, encoding);
        else if(decode->parsed()) return decodeCommand(inFile, outFile, encoding);

        std::cout << app.help() << std::endl;
    } catch (const CLI::ParseError &e) { return app.exit(e); }
    catch (const std::runtime_error &e) {
        const CLI::Error cli("Runtime Error", e.what());
        return app.exit(cli);
    }

    return 0;
}