import uproot 
import numpy as np
import pandas as pd
import plotly.express as px

file = uproot.open("/shared/physics/physdata/nuclear/ePIC_EIC/vd613/Internship/GEANT4/output/B4_latest0.root")
tree = file["B4_ScintillatorData;1"]
df = tree.arrays(library = "np")

# ------ Create a 2D histogram of beamY and beamX ------
beamY = df["beamY"]
beamX = df["beamX"]

fig = px.density_heatmap(df,"beamX", "beamY")
fig.show()