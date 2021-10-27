OUTDIR = $(shell pwd)/bin
DIRS = Meta Plugins Vps2GStreamer VpsUtilities

all:
	$(foreach var, $(DIRS), make -C $(var)  OUTDIR=$(OUTDIR);)
	dotnet build VPService/VPService.csproj --output $(OUTDIR)

clean:
	$(foreach var, $(DIRS), make clean -C $(var)  OUTDIR=$(OUTDIR);)
	@rm -f $(OUTDIR)/VPService.*
	@rm -f $(OUTDIR)/appsettings.*

run:
	cd $(OUTDIR) && GST_PLUGIN_PATH=./ LD_PRELOAD="./libgstvpsxprotect.so ./libgstvpsonvifmeta.so ./libgstvpsxprotectmeta.so ./libvpsutilities.so" dotnet VPService.dll && cd ..
