# Safetch
A simple, fast, and rock-solid fetch program.
## How to compile:
1. Clone the repository.
2. Enter the cloned repository folder.
3. Compile with whatever compiler you want. I use GCC: `gcc -o safetch safetch.c`

Or, use the binary in the releases (compiled using GCC)

## How to set a custom ASCII art
Initially, safetch does not come with any form of ASCII art next to the fetch data, whether it's something decorative or a distro ASCII art. Safetch gives users freedom on what ASCII art to use initially.

So, to set a custom ASCII art, make a folder called `safetch` in `~/.config`, and make a file called `ascii-art` in that folder. (full path will be ~/.config/safetch/ascii-art). Use any text editor to paste in your designated choice of ASCII art. Save it, and you now have ASCII art next to your fetch information.
