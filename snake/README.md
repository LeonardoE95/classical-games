# Snake in C

Custom implementation of snake in C using `SDL2` and `SDL2_TTF` made for educational purposes. After writing the code I have made a youtube playlist in which I explain all of the code, step by step, in italian. You can find the videos here

- [Snake in C 0/10 – Introduzione](https://www.youtube.com/watch?v=v6vn3qG_rwU)
- [Snake in C 1/10 – Configuratione SDL2](https://www.youtube.com/watch?v=_fOGAM9G7Zc)
- [Snake in C 2/10 – Finestre con SDL2](https://www.youtube.com/watch?v=Bz75tcpDyTA)
- [Snake in C 3/10 – Disegno griglia](https://www.youtube.com/watch?v=II0m7vW2ku8)
- [Snake in C 4/10 – Gestione cibo](https://www.youtube.com/watch?v=BLD8OsfKf4s)
- [Snake in C 5/10 – Gestione snake parte 1](https://www.youtube.com/watch?v=5MZOKOyOaa4)
- [Snake in C 6/10 – Gestione snake parte 2](https://www.youtube.com/watch?v=eS3ddKN80hg)
- [Snake in C 7/10 – Gestione ostacoli](https://www.youtube.com/watch?v=m0lPCfJmx3s)
- [Snake in C 8/10 – Gestione colori](https://www.youtube.com/watch?v=GrFxI3-UUS4)
- [Snake in C 9/10 – Gestione punteggio](https://www.youtube.com/watch?v=mXX1v0e10YY)
- [Snake in C 10/10 – Conclusione](https://www.youtube.com/watch?v=M9pgVanSl5g)

# Dependencies

The only dependencies are `SDL2` and `SDL2_TTF`. In archlinux these can be installed with

```
sudo pacman -S sdl2
sudo pacman -S sdl2_ttf
```

While in ubuntu

```
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-ttf-dev
```

Other than that, you need to specify the path to a `.ttf` file in one of the parameters at the top of the code called `FONT_PATH`. Also, this probably does not work on Windows (OS) because of the `gettimeofday()` function used for managing delays.

# Compilation

Once you have the dependencies install simply do

```
make
./main
```

and it should run.
