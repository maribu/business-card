.PHONY: all clean

all: output/gerbers.zip

output/gerbers.zip: business-card.kicad_sch panel.kicad_pcb
	kikit fab jlcpcb --no-drc --assembly --field MPN --schematic $^ $(dir $@)

panel.kicad_pcb: kikit.json business-card.kicad_pcb
	kikit panelize -p $^ $@

clean:
	rm -rf output
	rm -f panel.kicad_dru panel.kicad_pcb panel.kicad_prl panel.kicad_pro
