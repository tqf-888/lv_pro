all:
	echo "$(MAKE) -C wfd_player ARCH=$(ARCH) AR=$(AR) CC=$(CC) CPP=$(CPP)"
	$(MAKE) -C wfd_player \
		ARCH="$(ARCH)" \
		AR="$(AR)" \
		CC="$(CC)" \
		CPP="$(CXX)"

	echo "$(MAKE) -C wpa_client ARCH=$(ARCH) AR=$(AR) CC=$(CC) CPP=$(CPP)"
	$(MAKE) -C wpa_client \
		ARCH="$(ARCH)" \
		AR="$(AR)" \
		CC="$(CC)" \
		CPP="$(CXX)"

	echo "$(MAKE) -C libwfd2 ARCH=$(ARCH) AR=$(AR) CC=$(CC) CPP=$(CPP)"
	$(MAKE) -C libwfd2 \
		ARCH="$(ARCH)" \
		AR="$(AR)" \
		CC="$(CC)" \
		CPP="$(CXX)"

clean:
	$(MAKE) -C wfd_player clean
	$(MAKE) -C wpa_client clean
	$(MAKE) -C libwfd2 clean

.PHONY: all clean
