.PHONY: clean All

All:
	@echo "----------Building project:[ rleCompress - Debug ]----------"
	@cd "rleCompress" && "$(MAKE)" -f  "rleCompress.mk"
clean:
	@echo "----------Cleaning project:[ rleCompress - Debug ]----------"
	@cd "rleCompress" && "$(MAKE)" -f  "rleCompress.mk" clean
