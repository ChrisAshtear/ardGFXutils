.PHONY: clean All

All:
	@echo "----------Building project:[ rleDecompress - Debug ]----------"
	@cd "rleDecompress" && "$(MAKE)" -f  "rleDecompress.mk"
clean:
	@echo "----------Cleaning project:[ rleDecompress - Debug ]----------"
	@cd "rleDecompress" && "$(MAKE)" -f  "rleDecompress.mk" clean
