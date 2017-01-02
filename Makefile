# Call the project makefile in a subdirectory
.PHONY: FORCE    # phony target used if none specified on make cmd line
$(MAKECMDGOALS) FORCE:
	$(MAKE) -C make $(MAKEFLAGS) $(MAKECMDGOALS)

