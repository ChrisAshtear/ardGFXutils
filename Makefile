.PHONY: clean All

All:
	@echo "----------Building project:[ rleComp1 - Debug ]----------"
	@cd "rleCompress" && "$(MAKE)" -f  "rleComp1.mk"
clean:
	@echo "----------Cleaning project:[ rleComp1 - Debug ]----------"
	@cd "rleCompress" && "$(MAKE)" -f  "rleComp1.mk" clean
