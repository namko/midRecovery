# PC simulation
# (executable only)
pc: TARGET=PC
pc: $(OBJDIR)/$(BINARY)

# Herotab C8/Dropad A8/Haipad M7/iBall Slide i7011 and compatibles
# (executable only)
703: TARGET=703
703: CXX=$(ARM-CXX)
703: $(OBJDIR)/$(BINARY)

# Coby 7024 and compatibles
# (executable only)
7024: TARGET=7024
7024: CXX=$(ARM-CXX)
7024: $(OBJDIR)/$(BINARY)

# Herotab C8/Dropad A8/Haipad M7/iBall Slide i7011 and compatibles
# (full recovery)
recovery_703: TARGET=703
recovery_703: CXX=$(ARM-CXX)
recovery_703: $(OBJDIR)/$(RECOVERY)

# Coby 7024 and compatibles
# (full recovery)
recovery_7024: TARGET=7024
recovery_7024: CXX=$(ARM-CXX)
recovery_7024: $(OBJDIR)/$(RECOVERY)

