

// pcsc
#include <winscard.h>

// libc
#include <cstdio>
#include <cstdlib>

// std
#include <memory>
#include <vector>
#include <iostream>


namespace{ // move in separate header
	inline std::string err_to_str(const decltype(SCARD_S_SUCCESS) err){
		std::string msg;
#ifdef SWITCH_ERR
#error "SWITCH_ERR already defined"
#endif
#define SWITCH_ERR(msg_, err_) case err_: msg = std::string(msg_) + " (" + std::to_string(err) + "(" #err_ "))"; break;
		switch (err) {
			SWITCH_ERR("Command successful", SCARD_S_SUCCESS);
			SWITCH_ERR("Internal error", SCARD_F_INTERNAL_ERROR);
			SWITCH_ERR("Command cancelled", SCARD_E_CANCELLED);
			SWITCH_ERR("Invalid handle.", SCARD_E_INVALID_HANDLE);
			SWITCH_ERR("Invalid parameter given.", SCARD_E_INVALID_PARAMETER);
			SWITCH_ERR("Invalid target given.", SCARD_E_INVALID_TARGET);
			SWITCH_ERR("Not enough memory.", SCARD_E_NO_MEMORY);
			SWITCH_ERR("Waited too long.", SCARD_F_WAITED_TOO_LONG);
			SWITCH_ERR("Insufficient buffer.", SCARD_E_INSUFFICIENT_BUFFER);
			SWITCH_ERR("Unknown reader specified.", SCARD_E_UNKNOWN_READER);
			SWITCH_ERR("Command timeout", SCARD_E_TIMEOUT);
			SWITCH_ERR("Sharing violation", SCARD_E_SHARING_VIOLATION);
			SWITCH_ERR("No smart card inserted", SCARD_E_NO_SMARTCARD);
			SWITCH_ERR("Unknown card", SCARD_E_UNKNOWN_CARD);
			SWITCH_ERR("Cannot dispose handle", SCARD_E_CANT_DISPOSE);
			SWITCH_ERR("Card protocol mismatch", SCARD_E_PROTO_MISMATCH);
			SWITCH_ERR("Subsystem not ready", SCARD_E_NOT_READY);
			SWITCH_ERR("Invalid value given", SCARD_E_INVALID_VALUE);
			SWITCH_ERR("System cancelled", SCARD_E_SYSTEM_CANCELLED);
			SWITCH_ERR("RPC transport error", SCARD_F_COMM_ERROR);
			SWITCH_ERR("Unknown error", SCARD_F_UNKNOWN_ERROR);
			SWITCH_ERR("Invalid ATR", SCARD_E_INVALID_ATR);
			SWITCH_ERR("Reader is unavailable", SCARD_E_READER_UNAVAILABLE);
			SWITCH_ERR("", SCARD_P_SHUTDOWN);
			SWITCH_ERR("PCI struct too small", SCARD_E_PCI_TOO_SMALL);
			SWITCH_ERR("Reader is unsupported", SCARD_E_READER_UNSUPPORTED);
			SWITCH_ERR("Reader already exists", SCARD_E_DUPLICATE_READER);
			SWITCH_ERR("Card is unsupported", SCARD_E_CARD_UNSUPPORTED);
			SWITCH_ERR("Service not available", SCARD_E_NO_SERVICE);
			SWITCH_ERR("Service was stopped", SCARD_E_SERVICE_STOPPED);
			//SWITCH_ERR("", SCARD_E_UNEXPECTED); // defined equal to SCARD_E_UNSUPPORTED_FEATURE on my platform
			SWITCH_ERR("", SCARD_E_ICC_CREATEORDER);
			SWITCH_ERR("", SCARD_E_DIR_NOT_FOUND);
			SWITCH_ERR("", SCARD_E_NO_DIR);
			SWITCH_ERR("", SCARD_E_NO_FILE);
			SWITCH_ERR("", SCARD_E_NO_ACCESS);
			SWITCH_ERR("", SCARD_E_WRITE_TOO_MANY);
			SWITCH_ERR("", SCARD_E_BAD_SEEK);
			SWITCH_ERR("", SCARD_E_INVALID_CHV);
			SWITCH_ERR("", SCARD_E_UNKNOWN_RES_MNG);
			SWITCH_ERR("", SCARD_E_NO_SUCH_CERTIFICATE);
			SWITCH_ERR("", SCARD_E_CERTIFICATE_UNAVAILABLE);
			SWITCH_ERR("Cannot find a smart card reader", SCARD_E_NO_READERS_AVAILABLE);
			SWITCH_ERR("", SCARD_E_COMM_DATA_LOST);
			SWITCH_ERR("", SCARD_E_NO_KEY_CONTAINER);
			SWITCH_ERR("", SCARD_E_SERVER_TOO_BUSY);
			SWITCH_ERR("Card is not supported", SCARD_W_UNSUPPORTED_CARD);
			SWITCH_ERR("Card is unresponsive", SCARD_W_UNRESPONSIVE_CARD);
			SWITCH_ERR("Card is unpowered", SCARD_W_UNPOWERED_CARD);
			SWITCH_ERR("Card was reset.", SCARD_W_RESET_CARD);
			SWITCH_ERR("Card was removed", SCARD_W_REMOVED_CARD);
			SWITCH_ERR("", SCARD_W_SECURITY_VIOLATION);
			SWITCH_ERR("", SCARD_W_WRONG_CHV);
			SWITCH_ERR("", SCARD_W_CHV_BLOCKED);
			SWITCH_ERR("", SCARD_W_EOF);
			SWITCH_ERR("", SCARD_W_CANCELLED_BY_USER);
			SWITCH_ERR("", SCARD_W_CARD_NOT_AUTHENTICATED);
			SWITCH_ERR("Feature not supported", SCARD_E_UNSUPPORTED_FEATURE);
			default:
				msg = "Unknown error code (" + std::to_string(err) + ")"; break;
		}
#undef SWITCH_ERR
		return msg;
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
		decltype(SCARD_S_SUCCESS) res{};
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

	struct scard_connect_res{
		decltype(SCARD_S_SUCCESS) res{};
		SCARDHANDLE_handle handle{};
		decltype(SCARD_PROTOCOL_UNDEFINED) protocol{};
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
		decltype(SCARD_S_SUCCESS) res{};
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
	DWORD dwReaders = 0;
	const auto res2 = SCardListReaders(hContext.get(), nullptr, nullptr, &dwReaders);
	if(res2 != SCARD_S_SUCCESS){
		std::cerr << err_to_str(res2) << "\n";
		return 1;
	}

	std::string mszReaders(dwReaders, '\0');
	const auto res3 = SCardListReaders(hContext.get(), nullptr, &mszReaders[0], &dwReaders);
	if(res3 != SCARD_S_SUCCESS){
		std::cerr << err_to_str(res3) << "\n";
		return 1;
	}

	auto res4 = scard_connect(hContext.get(), mszReaders.c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1);
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

	BYTE cmd1[] = { 0x00, /* message to send */ };
	const auto res5 = scard_transmit(hCard.get(), &pioSendPci, cmd1, nullptr);
	if(res5.res != SCARD_S_SUCCESS){
		std::cerr << err_to_str(res5.res) << "\n";
		return 1;
	}
	return 0;
}

