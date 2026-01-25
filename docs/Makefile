SPHINXBUILD   = sphinx-build
SOURCEDIR     = source
BUILDDIR      = build

html:
	@$(SPHINXBUILD) -b html "$(SOURCEDIR)" "$(BUILDDIR)/html"
	@echo "HTML gerado em $(BUILDDIR)/html/"

clean:
	rm -rf $(BUILDDIR)/*

.PHONY: html clean
