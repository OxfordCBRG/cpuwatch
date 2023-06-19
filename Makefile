version = 1.4
release = 4
all : cpuwatch
cpuwatch : cpuwatch.cpp 
	g++ -O3 -Wall -Wextra -Wpedantic -Werror -std=c++20 -o cpuwatch cpuwatch.cpp 
clean :
	rm -rf cpuwatch cpuwatch_$(version)-$(release) cpuwatch_$(version)-$(release).deb
fake_install :
	install -m 755 -D cpuwatch $(prefix)/usr/sbin/cpuwatch
	install -m 644 -D LICENSE $(prefix)/usr/share/doc/cpuwatch/LICENSE
	install -m 644 -D README.md $(prefix)/usr/share/doc/cpuwatch/README.md
	install -m 644 -D cpuwatch.service $(prefix)/usr/lib/systemd/system/cpuwatch.service
install : prefix = /
install : fake_install
deb : prefix = ./cpuwatch_$(version)-$(release)/
deb : all fake_install
	sed -i 's/Version: .*/Version: $(version)-$(release)/' DEBIAN/control
	cp -ar DEBIAN cpuwatch_$(version)-$(release)
	dpkg-deb --build --root-owner-group cpuwatch_$(version)-$(release)
