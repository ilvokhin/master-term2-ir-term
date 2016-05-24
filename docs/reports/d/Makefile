# Makefile for LaTeX presentation

main = src/main.tex
tex = src/*.tex
pdf = main.pdf
pics = pics

# targets
all: $(pdf)

read:
	open $(pdf)

main.pdf: $(main) $(tex) $(pics)
	latexmk -pdf -pdflatex="pdflatex -interactive=nonstopmode" -use-make $(main)

clean:
	latexmk -c $(pdf)
	rm -rf *.nav *.snm

.PHONY: all read clean
