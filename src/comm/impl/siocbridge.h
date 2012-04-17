#include "../lanserialbridge.h"
#include <set>

namespace comm {
namespace sioc {

namespace control {
enum type
{
	reset = 0xF0,	// 240
	init,			// 241
	add				// 242
};
}

class SiocBridge: public LanSerialBridge
{
public:
	SiocBridge();
	virtual ~SiocBridge();

protected:
	virtual bool getString( comm::Command cmd, std::string *out );
	virtual void getCmd( const std::string &stream, std::vector<comm::Command> *cmds );

private:
	std::set<unsigned char> _cmds;
	bool _init;

};

}
}
