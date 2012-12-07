
#include <declarations.h>
#include <Parsers/TokenGenerator.h>

class MapReader {

private:
    std::string line;
    std::string key;
    std::vector <std::string> values;
    char key_delimiter;
    char val_delimiter;

    size_t __cpos__;
    size_t __vsiz__;

    void parseKey();
    void parseNextValue();

public:
    MapReader(
        const char keyDelimiter=':'
        , const char valDelimiter=',');

    void readLine(const std::string& line);
    inline const std::string& getKey () { return key; }
    inline const std::vector <std::string>& getValues () { return values; }

    inline void reset() {
        values.clear();
        line = EMPTY;
        __cpos__ = 0;
    }

    inline bool positionIsValid() {
        return __cpos__ <= line.length()-1;
    }

    inline bool valuesIncreased() {
        if (__vsiz__ == values.size()) return false;
        ++__vsiz__;
        return true;
    }

};

class MapWriter {
private:
    std::stringstream streambuf;
public:
    MapWriter (const std::string& mapName);
    MapWriter (const std::string&& mapName);

    void writeLine (const std::string& key, const std::string& value);

};
