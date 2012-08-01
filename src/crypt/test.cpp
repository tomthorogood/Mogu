#include <PacketCenter.h>

#include <TurnLeftLib/TurnLeft.h>
using TurnLeft::Utils::trimchar;

#include <BlowfishKey.h>

#include <iostream>
using std::cout;
using std::endl;

int main()
{
	string test_string = "Hello, my name is Tom?";
	BlowfishKeyCreator keysafe;
	BF_KEY* key = keysafe.getKey();
	PacketCenter handler(test_string, DECRYPTED);
	string encryption = handler.giveKey(key).encrypt();
	cout << encryption << endl;

	PacketCenter d_handler(encryption, ENCRYPTED);
	string decryption = d_handler.giveKey(key).decrypt();
	cout << decryption << endl;

	return 0;
}
