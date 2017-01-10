

// pcsc
#include "pcsc.hpp"

#include <cassert>
#include <stdexcept>


int main(){
	auto res1 = scard_establish_context(SCARD_SCOPE_SYSTEM);
	if(res1.first != SCARD_S_SUCCESS){
		std::cerr << scard_err_to_str(res1.first) << "\n";
		return 1;
	}
	unique_SCARDCONTEXT hContext = std::move(res1.second);

	auto readers = scard_list_reader(hContext.get());
	if(readers.first != SCARD_S_SUCCESS){
		std::cerr << "unable to lis readers " << scard_err_to_str(readers.first) << "\n";
		return 1;
	}
	if(readers.second.empty()){
		std::cout << "apparently there are no cardreaders\n";
	}

	std::cout << "Found following readers:\n";
	for(const auto& r : readers.second){
		std::cout << "\t" << r << "\n";
	}
	auto res4 = scard_connect(hContext.get(), readers.second.at(0).c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1);
	if(res4.first != SCARD_S_SUCCESS){
		std::cerr << "unable to connect to the first reader " << scard_err_to_str(res4.first) << "\n";
		return 1;
	}
	std::cout << "Succesfully connected to the first reader\n";

	unique_SCARDHANDLE hCard = std::move(res4.second.handle);
	const SCARD_IO_REQUEST* pioSendPci{};
	switch(res4.second.protocol)
	{
		case SCARD_PROTOCOL_T0:
			pioSendPci = SCARD_PCI_T0;
			break;
		case SCARD_PROTOCOL_T1:
			pioSendPci = SCARD_PCI_T1;
			break;
		case SCARD_PROTOCOL_RAW:
			pioSendPci = SCARD_PCI_RAW;
			break;
		case SCARD_PROTOCOL_UNDEFINED:
			std::cerr << "Protocol not defined\n";
			return 1;
		default:
			std::cerr << "unknown protocol\n";
			return 1;
	}
	std::cout << "The supported protocoll is " << proto_to_str(res4.second.protocol) << "\n";

	auto res_status = scard_status(hCard.get());
	if(res_status.first != SCARD_S_SUCCESS){
		std::cerr << "unable to get status of reader" << scard_err_to_str(res_status.first) << "\n";
		return 1;
	}

	/*
	BYTE cmd1[] = { }; // message to send
	DWORD respsize = ...
	const auto res5 = scard_transmit(hCard.get(), pioSendPci, cmd1, resp_size);
	if(res5.first != SCARD_S_SUCCESS){
		std::cerr << err_to_str(res5.res) << "\n";
		return 1;
	}
	*/
	return 0;
}

