pkgname=battleships
pkgver=0.0.1
pkgrel=1
epoch=0
pkgdesc="Game of battleships online"
arch=("x86_64")
url="https://github.com/szymon-jozef/battleships"
license=('GPL-3.0')
groups=()
depends=("spdlog" "raylib" "boost")
makedepends=("clang" "cmake")
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=()
noextract=()
md5sums=() #generate with 'makepkg -g'

pkgver() {
    grep -e "project.*VERSION" $startdir/CMakeLists.txt | awk 'NR==1' | awk '{print $3}' | tr -d ')'
}

prepare() {
	cd "$startdir/"
}

build() {
	cd "$startdir"
    cmake -B build --install-prefix /usr -D CMAKE_BUILD_TYPE=Release
    cmake --build build
}

package() {
	cd "$startdir"
    DESTDIR="$pkgdir" cmake  --install build --strip
}
