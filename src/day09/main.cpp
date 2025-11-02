#include "array2d.h"
#include "input.h"
#include "print.h"
#include "timer.h"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <numeric>
#include <optional>
#include <print>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

    struct Space {
        int32_t fileId;
        int32_t size;
    };

    struct Block {
        int32_t fileId{UNOCCUPIED_FILE_ID};

        static constexpr int32_t UNOCCUPIED_FILE_ID = -1;
    };

    class DiskMap {
      public:
        DiskMap(uint64_t size) : _blocks(size) {
        }

        uint64_t getSize() const {
            return _blocks.size();
        }

        void setBlock(uint64_t startId, uint64_t length, int32_t fileId) {
            if (startId + length > _blocks.size()) {
                throw std::runtime_error(std::format(
                    "Block does not fit in the DiskMap: startId={} length={} fileId={}", startId, length, fileId));
            }
            std::for_each(std::next(_blocks.begin(), startId), std::next(_blocks.begin(), startId + length),
                          [fileId](Block &block) {
                              block.fileId = fileId;
                          });
        }

        void print() const {
            for (const auto &block : _blocks) {
                std::cout << ((block.fileId == Block::UNOCCUPIED_FILE_ID) ? ".|" : std::format("{}|", block.fileId));
            }
            std::cout << "\n";
        }

        bool empty() const noexcept {
            return _blocks.empty();
        }

        uint64_t checksum() const {
            uint64_t sum = 0;
            for (size_t i = 0; i < _blocks.size(); ++i) {
                if (_blocks[i].fileId != Block::UNOCCUPIED_FILE_ID) {
                    sum += i * _blocks[i].fileId;
                }
            }
            return sum;
        }

        auto begin() noexcept {
            return _blocks.begin();
        }
        auto end() noexcept {
            return _blocks.end();
        }

        auto begin() const noexcept {
            return _blocks.begin();
        }
        auto end() const noexcept {
            return _blocks.end();
        }

        auto cbegin() const noexcept {
            return _blocks.cbegin();
        }
        auto cend() const noexcept {
            return _blocks.cend();
        }

        Block &operator[](size_t index) {
            return _blocks[index];
        }
        const Block &operator[](size_t index) const {
            return _blocks[index];
        }

      private:
        std::vector<Block> _blocks;
    };

    /**
     * Go over the input and find out the size of the DiskMap.
     */
    class DiskMapBuilder {
      public:
        DiskMap build(std::vector<uint8_t> input) {
            uint64_t numBlocks = std::accumulate(input.begin(), input.end(), 0ull);
            DiskMap map{numBlocks};

            int index = 0;
            int fileId = 0;
            int blockId = 0;

            std::for_each(input.begin(), input.end(), [&map, &index, &blockId, &fileId](uint8_t digit) {
                bool isFile = index % 2 == 0;
                if (isFile) {
                    if (digit == 0) {
                        std::cout << "Empty file!" << "\n";
                    }
                    map.setBlock(blockId, digit, fileId);
                    fileId++;
                }
                blockId += digit;
                index++;
            });

            return map;
        }
    };

    DiskMap loadInput(const std::string &filename) {
        auto lines = input::readFile<std::vector<std::string>>(filename, input::readLines);
        auto &line = lines[0];
        input::trim(line);

        std::vector<uint8_t> digits{};
        std::transform(line.cbegin(), line.cend(), std::back_inserter(digits), [](const auto &digit) {
            if (!std::isdigit(digit)) {
                throw std::runtime_error(std::format("Invalid input: expected a digit, found: {}", digit));
            }
            return digit - '0';
        });

        DiskMapBuilder builder{};
        return builder.build(digits);
    }

} // namespace

namespace part1 {

    void shiftBlocks(DiskMap &diskMap) {
        if (diskMap.empty())
            return;

        auto emptyBlockIt = diskMap.begin();
        auto fileBlockItRev = std::prev(diskMap.end());

        while (emptyBlockIt < fileBlockItRev) {
            // Find next empty block from the front
            while (emptyBlockIt < diskMap.end() && emptyBlockIt->fileId != Block::UNOCCUPIED_FILE_ID) {
                ++emptyBlockIt;
            }
            // Find next occupied block from the back
            while (fileBlockItRev > diskMap.begin() && fileBlockItRev->fileId == Block::UNOCCUPIED_FILE_ID) {
                --fileBlockItRev;
            }
            if (emptyBlockIt < fileBlockItRev) {
                // Move file block to empty block
                emptyBlockIt->fileId = fileBlockItRev->fileId;
                fileBlockItRev->fileId = Block::UNOCCUPIED_FILE_ID;
            }
        }
    }

    template <typename Input>
    void execute(const Input &input) {
        DiskMap diskMap = input;
        std::print("Disk Map Size: {}\n", diskMap.getSize());

        shiftBlocks(diskMap);

        std::cout << diskMap.checksum() << "\n";
    }
} // namespace part1

namespace part2 {

    enum class SpanType { Empty, File };

    struct DiskSpan {
        uint64_t start;
        uint64_t length;
        int32_t fileId{Block::UNOCCUPIED_FILE_ID};

        std::string toString() const {
            return std::format("<DiskSpan start={} length={} fileId={}>", start, length, fileId);
        }
    };

    struct ByStart {
        bool operator()(const DiskSpan &lhs, const DiskSpan &rhs) const noexcept {
            return lhs.start < rhs.start;
        }
    };

    struct ByStartReverse {
        bool operator()(const DiskSpan &lhs, const DiskSpan &rhs) const noexcept {
            return lhs.start > rhs.start;
        }
    };

    template <typename SpanCompare>
    void fillSpans(std::set<DiskSpan, SpanCompare> &spans, const DiskMap &diskMap, SpanType type) {
        bool isInSpan = false;
        uint64_t blockStartId = 0;
        uint64_t numBlocks = 0;
        int32_t currentFileId = Block::UNOCCUPIED_FILE_ID;

        for (size_t blockId = 0; blockId < diskMap.getSize(); blockId++) {
            const auto &block = diskMap[blockId];

            bool isEmptyBlock = (block.fileId == Block::UNOCCUPIED_FILE_ID);
            bool matchesSpanType = (type == SpanType::Empty) ? isEmptyBlock : !isEmptyBlock;

            if (matchesSpanType) {
                if (!isInSpan) {
                    isInSpan = true;
                    blockStartId = blockId;
                    numBlocks = 0;
                    currentFileId = block.fileId;
                } else if (type == SpanType::File && block.fileId != currentFileId) {
                    // File ID changed, so close previous file span and start a new one
                    spans.insert(DiskSpan{.start = blockStartId, .length = numBlocks, .fileId = currentFileId});
                    blockStartId = blockId;
                    numBlocks = 0;
                    currentFileId = block.fileId;
                }
                numBlocks++;
            } else {
                if (isInSpan) {
                    spans.insert(DiskSpan{.start = blockStartId, .length = numBlocks, .fileId = currentFileId});
                    isInSpan = false;
                }
            }
        }
        if (isInSpan) {
            spans.insert(DiskSpan{.start = blockStartId, .length = numBlocks, .fileId = currentFileId});
        }
    }

    void defragmentSpans(std::set<DiskSpan, ByStartReverse> &fileSpans, std::set<DiskSpan, ByStart> &emptySpans) {
        for (auto fileIt = fileSpans.begin(); fileIt != fileSpans.end();) {
            DiskSpan fileSpan = *fileIt;
            bool moved = false;

            for (auto emptyIt = emptySpans.begin(); emptyIt != emptySpans.end();) {
                const DiskSpan emptySpan = *emptyIt;

                if (emptySpan.start >= fileSpan.start)
                    break; // No available empty span. Keep the FileSpan where it is.
                if (emptySpan.length < fileSpan.length) {
                    ++emptyIt;
                    continue; // The empty span is too small. Keep searching.
                }

                emptyIt = emptySpans.erase(emptyIt);

                if (emptySpan.length > fileSpan.length) {
                    DiskSpan emptySpanRemainder{.start = emptySpan.start + fileSpan.length,
                                                .length = emptySpan.length - fileSpan.length};
                    emptySpans.insert(emptySpanRemainder);
                }

                DiskSpan movedFile{.start = emptySpan.start, .length = fileSpan.length, .fileId = fileSpan.fileId};

                fileIt = fileSpans.erase(fileIt);
                fileSpans.insert(movedFile);

                moved = true;
                break;
            }

            if (!moved) {
                ++fileIt;
            }
        }
    }

    void reconstruct(DiskMap &diskMap, const std::set<DiskSpan, ByStartReverse> fileSpans) {
        for (auto &block : diskMap) {
            block.fileId = Block::UNOCCUPIED_FILE_ID;
        }
        for (const auto &span : fileSpans) {
            for (uint64_t i = span.start; i < span.start + span.length; i++) {
                diskMap[i].fileId = span.fileId;
            }
        }
    }

    void defragment(DiskMap &diskMap) {
        std::set<DiskSpan, ByStart> emptySpans{};
        fillSpans(emptySpans, diskMap, SpanType::Empty);

        std::set<DiskSpan, ByStartReverse> fileSpans{};
        fillSpans(fileSpans, diskMap, SpanType::File);

        defragmentSpans(fileSpans, emptySpans);

        reconstruct(diskMap, fileSpans);

        // std::puts("Empty spans:");
        // for (const auto &span : emptySpans)  {
        //     std::cout << "\t" << span.toString() << "\n";
        // }

        // std::puts("File spans:");
        // for (const auto &span : fileSpans) {
        //     std::cout << "\t" << span.toString() << "\n";
        // }
    }

    template <typename Input>
    void execute(const Input &input) {
        DiskMap diskMap = input;

        defragment(diskMap);

        std::cout << diskMap.checksum() << "\n";
    }
} // namespace part2

int main() {
    auto input = loadInput("day09.txt");

    Timer timer;
    part1::execute(input);
    std::cout << "[Part 1] Time elapsed: " << timer.elapsed() << " seconds\n";

    timer.reset();
    part2::execute(input);
    std::cout << "[Part 2] Time elapsed: " << timer.elapsed() << " seconds\n";

    return 0;
}
