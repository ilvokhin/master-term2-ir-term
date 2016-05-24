# Makefile for LaTeX presentation

tex = src/main.tex
pdf = main.pdf

# targets
all: $(pdf)

read:
	open $(pdf)

main.pdf: $(tex)
	latexmk -pdf -pdflatex="pdflatex -interactive=nonstopmode" -use-make $(tex)

clean:
	latexmk -c $(pdf)
	rm -rf *.nav *.snm

.PHONY: all read clean
