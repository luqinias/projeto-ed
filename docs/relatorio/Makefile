PDFLATEX = pdflatex
ARQUIVO = relatorio

.PHONY: pdf clean

pdf:
	$(PDFLATEX) $(ARQUIVO).tex
	$(PDFLATEX) $(ARQUIVO).tex

clean:
	rm -f $(ARQUIVO).aux $(ARQUIVO).log $(ARQUIVO).toc $(ARQUIVO).out \
	      $(ARQUIVO).lof $(ARQUIVO).lot $(ARQUIVO).bbl $(ARQUIVO).blg

distclean: clean
	rm -f $(ARQUIVO).pdf
