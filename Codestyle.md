# Overview
description for the coding style used in QBeOS

* follow camel case
* DON'T USE ABBREVIATIONS.. my issue with os kernels written in c/c++ is that 
  they use a lot of abbreviations that renders the function naming useless. 
  Reading a function name should tell you what it does, which is not the case 
  for linux fx, unless you are a linux internal guru. In linux, if you see a function
  with a name you don't understand, you go on and read the function implementation
  and then you figure out that "Oh! ABC meant Allow Binary Combat" or whatever.
  using such abbreviations was OK during the time when variable names were limited in size,
  screens were small, and you needed to optimize in variable names to write readable code.
  This is not the case for the recent times.
* this one I guess is a basic c++ convention. all imports should happen in the .h
  file and only the .h file is imported to the cpp file corresponding to component.
* QBeOS follows an OOP paradigm. The linux kernel community dislikes c++ for some
  reason that I don't understand and probably will not. For now let's do OOP with
  c++ and see what the fuss is about.
* For the rest of coding style guides use: https://google.github.io/styleguide/cppguide.html.
Any point mentioned here outside the guide supersedes the referenced style guide.
