

// pcsc
#include <winscard.h>

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


namespace{ // move functions in separate module

	// not defined in winscard...
	using scard_res = decltype(SCARD_S_SUCCESS);

	inline std::string err_to_str(const scard_res err){
		std::stringstream ss;
		ss << std::ios::hex << std::setfill('0');
#ifdef FEK_CASE_ERR
#error "FEK_CASE_ERR already defined"
#endif
#define FEK_CASE_ERR(err_) case err_: ss << ""#err_; break;
		switch (err) {
			FEK_CASE_ERR(SCARD_S_SUCCESS);
			FEK_CASE_ERR(SCARD_F_INTERNAL_ERROR);
			FEK_CASE_ERR(SCARD_E_CANCELLED);
			FEK_CASE_ERR(SCARD_E_INVALID_HANDLE);
			FEK_CASE_ERR(SCARD_E_INVALID_PARAMETER);
			FEK_CASE_ERR(SCARD_E_INVALID_TARGET);
			FEK_CASE_ERR(SCARD_E_NO_MEMORY);
			FEK_CASE_ERR(SCARD_F_WAITED_TOO_LONG);
			FEK_CASE_ERR(SCARD_E_INSUFFICIENT_BUFFER);
			FEK_CASE_ERR(SCARD_E_UNKNOWN_READER);
			FEK_CASE_ERR(SCARD_E_TIMEOUT);
			FEK_CASE_ERR(SCARD_E_SHARING_VIOLATION);
			FEK_CASE_ERR(SCARD_E_NO_SMARTCARD);
			FEK_CASE_ERR(SCARD_E_UNKNOWN_CARD);
			FEK_CASE_ERR(SCARD_E_CANT_DISPOSE);
			FEK_CASE_ERR(SCARD_E_PROTO_MISMATCH);
			FEK_CASE_ERR(SCARD_E_NOT_READY);
			FEK_CASE_ERR(SCARD_E_INVALID_VALUE);
			FEK_CASE_ERR(SCARD_E_SYSTEM_CANCELLED);
			FEK_CASE_ERR(SCARD_F_COMM_ERROR);
			FEK_CASE_ERR(SCARD_F_UNKNOWN_ERROR);
			FEK_CASE_ERR(SCARD_E_INVALID_ATR);
			FEK_CASE_ERR(SCARD_E_READER_UNAVAILABLE);
			FEK_CASE_ERR(SCARD_P_SHUTDOWN);
			FEK_CASE_ERR(SCARD_E_PCI_TOO_SMALL);
			FEK_CASE_ERR(SCARD_E_READER_UNSUPPORTED);
			FEK_CASE_ERR(SCARD_E_DUPLICATE_READER);
			FEK_CASE_ERR(SCARD_E_CARD_UNSUPPORTED);
			FEK_CASE_ERR(SCARD_E_NO_SERVICE);
			FEK_CASE_ERR(SCARD_E_SERVICE_STOPPED);
			//FEK_CASE_ERR(SCARD_E_UNEXPECTED); // defined equal to SCARD_E_UNSUPPORTED_FEATURE on my platform
			FEK_CASE_ERR(SCARD_E_ICC_CREATEORDER);
			FEK_CASE_ERR(SCARD_E_DIR_NOT_FOUND);
			FEK_CASE_ERR(SCARD_E_NO_DIR);
			FEK_CASE_ERR(SCARD_E_NO_FILE);
			FEK_CASE_ERR(SCARD_E_NO_ACCESS);
			FEK_CASE_ERR(SCARD_E_WRITE_TOO_MANY);
			FEK_CASE_ERR(SCARD_E_BAD_SEEK);
			FEK_CASE_ERR(SCARD_E_INVALID_CHV);
			FEK_CASE_ERR(SCARD_E_UNKNOWN_RES_MNG);
			FEK_CASE_ERR(SCARD_E_NO_SUCH_CERTIFICATE);
			FEK_CASE_ERR(SCARD_E_CERTIFICATE_UNAVAILABLE);
			FEK_CASE_ERR(SCARD_E_NO_READERS_AVAILABLE);
			FEK_CASE_ERR(SCARD_E_COMM_DATA_LOST);
			FEK_CASE_ERR(SCARD_E_NO_KEY_CONTAINER);
			FEK_CASE_ERR(SCARD_E_SERVER_TOO_BUSY);
			FEK_CASE_ERR(SCARD_W_UNSUPPORTED_CARD);
			FEK_CASE_ERR(SCARD_W_UNRESPONSIVE_CARD);
			FEK_CASE_ERR(SCARD_W_UNPOWERED_CARD);
			FEK_CASE_ERR(SCARD_W_RESET_CARD);
			FEK_CASE_ERR(SCARD_W_REMOVED_CARD);
			FEK_CASE_ERR(SCARD_W_SECURITY_VIOLATION);
			FEK_CASE_ERR(SCARD_W_WRONG_CHV);
			FEK_CASE_ERR(SCARD_W_CHV_BLOCKED);
			FEK_CASE_ERR(SCARD_W_EOF);
			FEK_CASE_ERR(SCARD_W_CANCELLED_BY_USER);
			FEK_CASE_ERR(SCARD_W_CARD_NOT_AUTHENTICATED);
			FEK_CASE_ERR(SCARD_E_UNSUPPORTED_FEATURE);
			default: ss << "unknown error";
		}
#undef FEK_CASE_ERR
		ss << " (" << std::setw(sizeof(err)) << err << ")";
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
	const SCARDCONTEXT invalid_handle = {}; // assuming the 0-init handle is an "invalid" handle
	struct SCARDHANDLE_handle {
		SCARDHANDLE handle = invalid_handle;
		close_mode m = close_mode::leave_card;
		SCARDHANDLE_handle() = default;
		explicit SCARDHANDLE_handle(const SCARDHANDLE h, close_mode m_ = close_mode::leave_card) : handle(h), m(m_){}
		// copy
		SCARDHANDLE_handle(const SCARDHANDLE_handle&) = delete;
		SCARDHANDLE_handle& operator=( const SCARDHANDLE_handle& ) = delete;
		// move
		SCARDHANDLE_handle( SCARDHANDLE_handle&& o ) : handle(o.handle), m(o.m){
			o.handle = invalid_handle;
		}
		SCARDHANDLE_handle& operator=( SCARDHANDLE_handle&& o ){
			reset(o.release());
			handle = o.handle;
			m = o.m;
			o.handle = invalid_handle;
			return *this;
		}

		~SCARDHANDLE_handle(){
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
		void swap(SCARDHANDLE_handle& o){
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
	struct SCARDCONTEXT_handle {
		SCARDCONTEXT context = invalid_context;
		SCARDCONTEXT_handle() = default;
		explicit SCARDCONTEXT_handle(const SCARDCONTEXT c) : context(c){}
		// copy
		SCARDCONTEXT_handle(const SCARDCONTEXT_handle&) = delete;
		SCARDCONTEXT_handle& operator=( const SCARDCONTEXT_handle& ) = delete;
		// move
		SCARDCONTEXT_handle( SCARDCONTEXT_handle&& o ) : context(o.context){
			o.context = invalid_context;
		}
		SCARDCONTEXT_handle& operator=( SCARDCONTEXT_handle&& o ){
			reset(o.release());
			context = o.context;
			o.context = invalid_context;
			return *this;
		}

		~SCARDCONTEXT_handle(){
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
		void swap(SCARDCONTEXT_handle& o){
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
	struct scard_transmit_res{
		scard_res res{};
		std::vector<BYTE> response;
	};
	scard_transmit_res scard_transmit(SCARDHANDLE hCard, const SCARD_IO_REQUEST *pioSendPci, const arr_view& sendbuffer, SCARD_IO_REQUEST *pioRecvPci){
		scard_transmit_res toreturn;
		std::vector<BYTE> response(256);
		DWORD size = response.size();
		toreturn.res = SCardTransmit(hCard, pioSendPci, sendbuffer.data, sendbuffer.size, pioRecvPci, response.data(), &size);
		if(toreturn.res == SCARD_S_SUCCESS){
			response.resize(size);
			toreturn.response = response;
		}
		return toreturn;
	}

	using scard_protocol = decltype(SCARD_PROTOCOL_UNDEFINED);
	struct scard_connect_res{
		scard_res res{};
		SCARDHANDLE_handle handle{};
		scard_protocol protocol{};
	};
	scard_connect_res scard_connect(SCARDCONTEXT hContext, LPCSTR szReader, DWORD dwShareMode, DWORD dwPreferredProtocols){
		scard_connect_res toreturn;
		SCARDHANDLE h;
		DWORD protocol{};
		toreturn.res = SCardConnect(hContext, szReader, dwShareMode, dwPreferredProtocols, &h, &protocol);
		if(toreturn.res == SCARD_S_SUCCESS){
			toreturn.protocol = protocol;
			toreturn.handle.reset(h);
		}
		return toreturn;
	}



	struct scard_establish_context_res{
		scard_res res{};
		SCARDCONTEXT_handle handle{};
	};
	scard_establish_context_res scard_establish_context(const DWORD dwScope){
		scard_establish_context_res toreturn;
		SCARDCONTEXT context;
		toreturn.res = SCardEstablishContext(dwScope, nullptr, nullptr, &context);
		if(toreturn.res == SCARD_S_SUCCESS){
			toreturn.handle.reset(context);
		}
		return toreturn;
	}
}

int main()
{
	auto res1 = scard_establish_context(SCARD_SCOPE_SYSTEM);
	if(res1.res != SCARD_S_SUCCESS){
		std::cerr << err_to_str(res1.res) << "\n";
		return 1;
	}
	SCARDCONTEXT_handle hContext = std::move(res1.handle);

	// FIXME: move SCardListReaders to a single function call that handles the memory
	DWORD bufsize = 0;
	const auto res2 = SCardListReaders(hContext.get(), nullptr, nullptr, &bufsize);
	if(res2 != SCARD_S_SUCCESS){
		std::cerr << err_to_str(res2) << "\n";
		return 1;
	}

	std::vector<char> buffer(bufsize);
	const auto res3 = SCardListReaders(hContext.get(), nullptr, buffer.data(), &bufsize);
	if(res3 != SCARD_S_SUCCESS){
		std::cerr << err_to_str(res3) << "\n";
		return 1;
	}
	buffer.resize(bufsize); // buffer is double - '\0' -terminated

	std::vector<std::string> readers;
	{
		auto pdata = buffer.data();
		while(*pdata != '\0'){
			const auto len = std::strlen(pdata);
			std::string tmp(pdata, len);
			readers.push_back(tmp);
			pdata += (len + 1);
		}
	}

	auto res4 = scard_connect(hContext.get(), readers.at(0).c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1);
	if(res4.res != SCARD_S_SUCCESS){
		std::cerr << err_to_str(res4.res) << "\n";
		return 1;
	}
	SCARDHANDLE_handle hCard = std::move(res4.handle);
	SCARD_IO_REQUEST pioSendPci{};
	switch(res4.protocol)
	{
		case SCARD_PROTOCOL_T0:
			pioSendPci = *SCARD_PCI_T0;
			break;
		case SCARD_PROTOCOL_T1:
			pioSendPci = *SCARD_PCI_T1;
			break;
		case SCARD_PROTOCOL_RAW:
			pioSendPci = *SCARD_PCI_RAW;
			break;
		default:
			std::cerr << "unknown protocol\n";
			return 1;
	}

	/*
	BYTE cmd1[] = { }; // message to send
	const auto res5 = scard_transmit(hCard.get(), &pioSendPci, cmd1, nullptr);
	if(res5.res != SCARD_S_SUCCESS){
		std::cerr << err_to_str(res5.res) << "\n";
		return 1;
	}
	*/
	return 0;
}
