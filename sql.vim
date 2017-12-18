" Vim completion script
" Language:     PL/pgSQL
" Maintainer:   Gustavo Junior Alves <gjalves@gjalves.com.br>
" Last Change:  2017 Dec 16

fun! Pgscope_complete(findstart, base)
    if(a:findstart == 1) && (a:base == '')
        return 0
    endif

    for line in readfile("tags", '')
        if (strcharpart(line, 0, 1) != '!')
            let parts = split(line)
            if(stridx(parts[0], a:base) == 0)
                call complete_add(parts[0])
            endif
        endif
    endfor
    call complete_check()
endfun

set omnifunc=Pgscope_complete
