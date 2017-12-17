# pgscope
PL/pgSQL source code tree search and browse tool

Embed seamlessly with vim

# Getting Started

## Download

[Fedora 27 RPM Package](https://github.com/gjalves/pgscope/releases/download/0.0.2/pgscope-0.0.2-1.fc27.x86_64.rpm)

## Installation from source

```
git clone https://github.com/lfittl/libpg_query.git
cd libpg_query
make
cd -
make
mkdir ~/.vim/after/ftplugin/
cp sql.vim .vim/after/ftplugin/sql.vim
```

## Using

Type pgscope to index all .sql files in current and descending directories. After that, open your .sql file with vim and put cursor under any external function. Call it using <kbd>CTRL</kbd>+<kbd>]</kbd>. If you wish to came back to calling function, use <kbd>CTRL</kbd>+<kbd>o</kbd>

You can autocomplete functions using <kbd>CTRL</kbd>+<kbd>x</kbd> and <kbd>CTRL</kbd>+<kbd>o</kbd>
If you need to reindex, use `:!pgscope` in vim.

If you wish to reindex automatically after save, type in vim command mode:

```
:autocmd BufWritePost *.sql :silent !pgscope
```

[![asciicast](https://asciinema.org/a/uGW8Wz5TM3vqRoxCtqI0bCLxM.png)](https://asciinema.org/a/uGW8Wz5TM3vqRoxCtqI0bCLxM)

# Donation

Contributions of any amount are very welcome and will help the continuity of this software development

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=HNXL8RS8FWGB2)

# Author

[Gustavo Junior Alves](https://github.com/gjalves)

# License

PgScope is copyright 2017 for Gustavo Junior Alves and available under BSD 3-Clause License. See the [LICENSE](https://github.com/gjalves/pgscope/blob/master/LICENSE) file.
