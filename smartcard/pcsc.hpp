#ifndef FEK_PCSC_HPP
#define FEK_PCSC_HPP

// pcsc
#ifndef _WIN32
#include <pcsclite.h>
#endif
#include <winscard.h>

// libc
// libc
#include <cstdio>
#include <cstdlib>
#include <cstring>

// std
#include <memory>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace fek {

	// not defined in winscard...
	using scard_res = decltype(SCARD_S_SUCCESS);

	inline std::string err_to_str(const scard_res err){
		std::stringstream ss;
		switch (err) {
#ifdef FEK_CASE_ADD
#error "FEK_CASE_ADD already defined"
#endif
#define FEK_CASE_ADD(val__) case val__: ss << ""#val__; break;
			FEK_CASE_ADD(SCARD_S_SUCCESS);
			FEK_CASE_ADD(SCARD_F_INTERNAL_ERROR);
			FEK_CASE_ADD(SCARD_E_CANCELLED);
			FEK_CASE_ADD(SCARD_E_INVALID_HANDLE);
			FEK_CASE_ADD(SCARD_E_INVALID_PARAMETER);
			FEK_CASE_ADD(SCARD_E_INVALID_TARGET);
			FEK_CASE_ADD(SCARD_E_NO_MEMORY);
			FEK_CASE_ADD(SCARD_F_WAITED_TOO_LONG);
			FEK_CASE_ADD(SCARD_E_INSUFFICIENT_BUFFER);
			FEK_CASE_ADD(SCARD_E_UNKNOWN_READER);
			FEK_CASE_ADD(SCARD_E_TIMEOUT);
			FEK_CASE_ADD(SCARD_E_SHARING_VIOLATION);
			FEK_CASE_ADD(SCARD_E_NO_SMARTCARD);
			FEK_CASE_ADD(SCARD_E_UNKNOWN_CARD);
			FEK_CASE_ADD(SCARD_E_CANT_DISPOSE);
			FEK_CASE_ADD(SCARD_E_PROTO_MISMATCH);
			FEK_CASE_ADD(SCARD_E_NOT_READY);
			FEK_CASE_ADD(SCARD_E_INVALID_VALUE);
			FEK_CASE_ADD(SCARD_E_SYSTEM_CANCELLED);
			FEK_CASE_ADD(SCARD_F_COMM_ERROR);
			FEK_CASE_ADD(SCARD_F_UNKNOWN_ERROR);
			FEK_CASE_ADD(SCARD_E_INVALID_ATR);
			FEK_CASE_ADD(SCARD_E_READER_UNAVAILABLE);
			FEK_CASE_ADD(SCARD_P_SHUTDOWN);
			FEK_CASE_ADD(SCARD_E_PCI_TOO_SMALL);
			FEK_CASE_ADD(SCARD_E_READER_UNSUPPORTED);
			FEK_CASE_ADD(SCARD_E_DUPLICATE_READER);
			FEK_CASE_ADD(SCARD_E_CARD_UNSUPPORTED);
			FEK_CASE_ADD(SCARD_E_NO_SERVICE);
			FEK_CASE_ADD(SCARD_E_SERVICE_STOPPED);
			FEK_CASE_ADD(SCARD_E_UNSUPPORTED_FEATURE);
			//FEK_CASE_ADD(SCARD_E_UNEXPECTED); see default case
			FEK_CASE_ADD(SCARD_E_ICC_CREATEORDER);
			FEK_CASE_ADD(SCARD_E_DIR_NOT_FOUND);
			FEK_CASE_ADD(SCARD_E_NO_DIR);
			FEK_CASE_ADD(SCARD_E_NO_FILE);
			FEK_CASE_ADD(SCARD_E_NO_ACCESS);
			FEK_CASE_ADD(SCARD_E_WRITE_TOO_MANY);
			FEK_CASE_ADD(SCARD_E_BAD_SEEK);
			FEK_CASE_ADD(SCARD_E_INVALID_CHV);
			FEK_CASE_ADD(SCARD_E_UNKNOWN_RES_MNG);
			FEK_CASE_ADD(SCARD_E_NO_SUCH_CERTIFICATE);
			FEK_CASE_ADD(SCARD_E_CERTIFICATE_UNAVAILABLE);
			FEK_CASE_ADD(SCARD_E_NO_READERS_AVAILABLE);
			FEK_CASE_ADD(SCARD_E_COMM_DATA_LOST);
			FEK_CASE_ADD(SCARD_E_NO_KEY_CONTAINER);
			FEK_CASE_ADD(SCARD_E_SERVER_TOO_BUSY);
			FEK_CASE_ADD(SCARD_W_UNSUPPORTED_CARD);
			FEK_CASE_ADD(SCARD_W_UNRESPONSIVE_CARD);
			FEK_CASE_ADD(SCARD_W_UNPOWERED_CARD);
			FEK_CASE_ADD(SCARD_W_RESET_CARD);
			FEK_CASE_ADD(SCARD_W_REMOVED_CARD);
			FEK_CASE_ADD(SCARD_W_SECURITY_VIOLATION);
			FEK_CASE_ADD(SCARD_W_WRONG_CHV);
			FEK_CASE_ADD(SCARD_W_CHV_BLOCKED);
			FEK_CASE_ADD(SCARD_W_EOF);
			FEK_CASE_ADD(SCARD_W_CANCELLED_BY_USER);
			FEK_CASE_ADD(SCARD_W_CARD_NOT_AUTHENTICATED);
#undef FEK_CASE_ADD
			default:{
				// apparently pcsc-lite never returns SCARD_E_UNEXPECTED, and it's value is set equal to
				// SCARD_E_UNSUPPORTED_FEATURE -> we can't handle it in a switch if we wan't the same code
				// to compile on windows and other systems
				if(err == SCARD_E_UNEXPECTED){
					ss << "SCARD_E_UNEXPECTED";
				} else{
					ss << "unknown SCARD error value";
				}
			}
		}
		ss << " (0x" << std::hex << std::setfill('0')<< std::setw(sizeof(err)) << err << ")";
		auto res = ss.str();
		return res;
	}

	inline std::string proto_to_str(const DWORD prot){
		std::stringstream ss;
		switch (prot) {
#ifdef FEK_CASE_ADD
#error "FEK_CASE_ADD already defined"
#endif
#define FEK_CASE_ADD(val__) case val__: ss << ""#val__; break;
			FEK_CASE_ADD(SCARD_PROTOCOL_UNDEFINED);
			//FEK_CASE_ADD(SCARD_PROTOCOL_UNSET); // see defaul case
			FEK_CASE_ADD(SCARD_PROTOCOL_T1);
			FEK_CASE_ADD(SCARD_E_CANCELLED);
			FEK_CASE_ADD(SCARD_PROTOCOL_RAW);
			FEK_CASE_ADD(SCARD_PROTOCOL_T15);
#define SCARD_PROTOCOL_UNSET SCARD_PROTOCOL_UNDEFINED
#undef FEK_CASE_ERR
			default:{
				// apparently SCARD_PROTOCOL_UNSET is equal to SCARD_PROTOCOL_UNDEFINED for backward compatibility
				if(prot == SCARD_PROTOCOL_UNSET){
					ss << "SCARD_PROTOCOL_UNSET";
				} else{
					ss << "unknown SCARD protocol";
				}
			}
		}
		ss << " (0x" << std::hex << std::setfill('0')<< std::setw(sizeof(prot)) << prot << ")";
		auto res = ss.str();
		return res;
	}

	enum class close_mode : DWORD {
		// shoudl be decltype (SCARD_LEAVE_CARD) instead of DWORD, but SCardDisconnect takes a DWORD...
		leave_card = SCARD_LEAVE_CARD,
		reset_card = SCARD_RESET_CARD,
		unpower_card = SCARD_UNPOWER_CARD,
		eject_card = SCARD_EJECT_CARD,
	};

	// data structures similar for unique_ptr, but for smartcard handles
	const SCARDHANDLE invalid_handle = {}; // assuming the 0-init handle is an "invalid" handle
	struct unique_SCARDHANDLE {
		SCARDHANDLE handle = invalid_handle;
		close_mode m = close_mode::leave_card;
		unique_SCARDHANDLE() = default;
		explicit unique_SCARDHANDLE(const SCARDHANDLE h, close_mode m_ = close_mode::leave_card) : handle(h), m(m_){}
		// copy
		unique_SCARDHANDLE(const unique_SCARDHANDLE&) = delete;
		unique_SCARDHANDLE& operator=( const unique_SCARDHANDLE& ) = delete;
		// move
		unique_SCARDHANDLE( unique_SCARDHANDLE&& o ) : handle(o.handle), m(o.m){
			o.handle = invalid_handle;
		}
		unique_SCARDHANDLE& operator=( unique_SCARDHANDLE&& o ){
			reset(o.release());
			handle = o.handle;
			m = o.m;
			o.handle = invalid_handle;
			return *this;
		}

		~unique_SCARDHANDLE(){
			using u_type = std::underlying_type<close_mode>::type;
			const auto res = ::SCardDisconnect(handle, static_cast<u_type>(m));
			(void) res;
		}
		// modifiers
		SCARDHANDLE release(){
			const auto old = handle;
			handle = invalid_handle;
			return old;
		}

		void reset(SCARDHANDLE h = {}){
			const auto old = handle;
			handle = h;
			if(old != invalid_handle){
				using u_type = std::underlying_type<close_mode>::type;
				const auto res = ::SCardDisconnect(old, static_cast<u_type>(m));
				(void) res;
			}
		}
		void swap(unique_SCARDHANDLE& o){
			std::swap(handle, o.handle);
			std::swap(m, o.m);
		}

		// observers
		SCARDHANDLE get() const noexcept {
			return handle;
		}
		explicit operator bool() const{
			return handle != invalid_handle;
		}
	};

	const SCARDCONTEXT invalid_context = {}; // assuming the 0-init context is an "invalid" context
	struct unique_SCARDCONTEXT {
		SCARDCONTEXT context = invalid_context;
		unique_SCARDCONTEXT() = default;
		explicit unique_SCARDCONTEXT(const SCARDCONTEXT c) : context(c){}
		// copy
		unique_SCARDCONTEXT(const unique_SCARDCONTEXT&) = delete;
		unique_SCARDCONTEXT& operator=( const unique_SCARDCONTEXT& ) = delete;
		// move
		unique_SCARDCONTEXT( unique_SCARDCONTEXT&& o ) : context(o.context){
			o.context = invalid_context;
		}
		unique_SCARDCONTEXT& operator=( unique_SCARDCONTEXT&& o ){
			reset(o.release());
			context = o.context;
			o.context = invalid_context;
			return *this;
		}

		~unique_SCARDCONTEXT(){
			const auto res = ::SCardReleaseContext(context);
			(void) res;
		}
		// modifiers
		SCARDCONTEXT release(){
			const auto old = context;
			context = invalid_context;
			return old;
		}

		void reset(SCARDCONTEXT h = {}){
			const auto old = context;
			context = h;
			if(old != invalid_context){
				const auto res = ::SCardReleaseContext(old);
				(void) res;
			}
		}
		void swap(unique_SCARDCONTEXT& o){
			std::swap(context, o.context);
		}

		// observers
		SCARDCONTEXT get() const noexcept {
			return context;
		}
		explicit operator bool() const{
			return context != invalid_context;
		}
	};


	struct transaction_closer {
		SCARDHANDLE hCard;
		close_mode disposition;
		transaction_closer(SCARDHANDLE hCard_, close_mode disposition_ = close_mode::leave_card) :
		    hCard(hCard_), disposition(disposition_){
		}

		~transaction_closer(){
			if(hCard != invalid_handle){
				using u_type = std::underlying_type<close_mode>::type;
				const auto res = SCardEndTransaction(hCard, static_cast<u_type>(disposition));
				(void)res;
			}
		}

		void release(){
			hCard = invalid_handle;
		}

	};

	// non owning "view" of data (can be inside array, contigous container, pointed, ...)
	struct arr_view{
		const BYTE*  data = nullptr;
		const size_t size = 0;

		arr_view() noexcept = default;

		// if c++14 we could use a templated container with std::size...
		template<size_t N>
		arr_view(const BYTE (&data_)[N]) noexcept : data(data_), size(N){}

		arr_view(const BYTE* data_, const size_t size_) noexcept : data(data_), size(size_){}

		template<class seq_container>
		arr_view(const seq_container& c) : data{c.data()}, size{c.size()} {}
	};

	// like SCardTransmit, but takes an arr_view
	inline std::pair<scard_res, std::vector<BYTE>> scard_transmit(SCARDHANDLE hCard, const SCARD_IO_REQUEST *pioSendPci, const arr_view& sendbuffer, SCARD_IO_REQUEST *pioRecvPci){
		std::vector<BYTE> response(256);
		DWORD size = response.size();
		auto res = SCardTransmit(hCard, pioSendPci, sendbuffer.data, sendbuffer.size, pioRecvPci, response.data(), &size);
		if(res == SCARD_S_SUCCESS){
			response.resize(size);
			return {res, response};
		}
		return {res, {}};
	}

	struct scard_connect_res{
		unique_SCARDHANDLE handle{};
		DWORD protocol{};
	};
	inline std::pair<scard_res, scard_connect_res> scard_connect(SCARDCONTEXT hContext, LPCSTR szReader, DWORD dwShareMode, DWORD dwPreferredProtocols){
		scard_connect_res toreturn;
		SCARDHANDLE h;
		DWORD protocol{};
		auto res = SCardConnect(hContext, szReader, dwShareMode, dwPreferredProtocols, &h, &protocol);
		if(res == SCARD_S_SUCCESS){
			toreturn.protocol = protocol;
			toreturn.handle.reset(h);
		}
		return {res, std::move(toreturn)};
	}

	inline std::pair<scard_res, unique_SCARDCONTEXT> scard_establish_context(const DWORD dwScope){
		SCARDCONTEXT context;
		auto res = SCardEstablishContext(dwScope, nullptr, nullptr, &context);
		if(res == SCARD_S_SUCCESS){
			unique_SCARDCONTEXT h(context);
			return {res, std::move(h)};
		}
		return {res, unique_SCARDCONTEXT()};
	}

	// "explodes" a double '\0'-terminated list of z_str
	inline std::vector<std::string> explode(const char* zz_str){
		std::vector<std::string> to_return;
		auto pdata = zz_str;
		while(*pdata != '\0'){
			const auto len = std::strlen(pdata);
			if(len > 0){
				std::string tmp(pdata, len);
				to_return.push_back(tmp);
			}
			pdata += (len + 1);
		}
		return to_return;
	}

	struct scard_list_readers_res{
		scard_res res{};
		std::vector<std::string> readers;
	};

	inline std::pair<scard_res, std::vector<std::string>> scard_list_reader(SCARDCONTEXT hContext){
		DWORD bufsize = 0;
		auto res = SCardListReaders(hContext, nullptr, nullptr, &bufsize);
		if(res != SCARD_S_SUCCESS){
			return {res, {}};
		}

		std::vector<char> buffer(bufsize);
		res = SCardListReaders(hContext, nullptr, buffer.data(), &bufsize);
		if(res != SCARD_S_SUCCESS){
			return {res, {}};
		}
		buffer.resize(bufsize); // buffer is double - '\0' -terminated
		auto readers = explode(buffer.data());

		return {res, readers};
	}


	struct scard_status_res{
		DWORD state{};
		DWORD protocoll{};
		std::string reader;
		std::vector<unsigned char> atr;
	};

	inline std::pair<scard_res, scard_status_res> scard_status(SCARDHANDLE hCard){
		DWORD names_len{};
		DWORD atr_len{};
		auto res = SCardStatus(hCard, nullptr, &names_len, nullptr, nullptr, nullptr, &atr_len);
		if(res != SCARD_S_SUCCESS){
			return {res, {}};
		}
		scard_status_res toreturn;
		toreturn.reader.resize(names_len);
		toreturn.atr.resize(atr_len);
		DWORD state{};
		DWORD protocoll{};
		res = SCardStatus(hCard, &(toreturn.reader[0]), &names_len, &state, &protocoll, toreturn.atr.data(), &atr_len);
		if(res != SCARD_S_SUCCESS){
			return {res, {}};
		}
		toreturn.atr.resize(atr_len);
		toreturn.reader.resize(names_len);

		return {res, toreturn};
	}
}

#endif
