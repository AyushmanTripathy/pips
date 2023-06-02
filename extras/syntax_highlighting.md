# Syntax Highlighting

syntax highlighting in pipescript is pretty similar to python's so tweak your
editor config accordingly. for example, in vim

```vim
autocmd BufNewFile,BufRead *.pipescript set syntax=python
```

## VIM Setup

1. put `pipescript.vim` in `~/.vim` or `~/.config/nvim`
1. add this to your `.vimrc` or `init.vim`

```vim
" for neovim
autocmd BufRead,BufNewFile *.pipescript
      \ source $HOME/.config/nvim/pipescript.vim

" for vim
autocmd BufRead,BufNewFile *.pipescript
      \ source $HOME/.vim/pipescript.vim
```
3. make sure this line is below your `colorscheme` setting
