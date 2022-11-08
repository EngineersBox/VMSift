#%%
import re, os
from typing import List, Dict
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt

UNICODE_HEATMAP_REGEX = re.compile("%\)([^T=]*?)\s\sTotal")

def convertOrdinal(ordinal: int) -> int:
    if (ordinal < 9600):
        return 0
    return ordinal - 9600

def parseSnapshots(directoryPath: str, properties: List[str], threshold: int) -> List[Dict[str, List[int]]]:
    results: List[Dict[str, List[int]]] = [None] * 260
    for snapshot in [f for f in os.listdir(directoryPath) if os.path.isfile(os.path.join(directoryPath, f))]:
        index = int(snapshot.split(".")[0])
        results[index] = {prop:[] for prop in properties}
        with open(f"{directoryPath}/{snapshot}", "r", encoding='utf-8') as file:
            propsIndex = 0
            fileData = file.read().replace("\n", "")
            groups = UNICODE_HEATMAP_REGEX.findall(fileData)
            for group in groups:
                parsed = [convertOrdinal(ord(c)) for c in group]
                if (len(parsed) < threshold):
                    parsed.extend([0] * (threshold - len(parsed)))
                results[index][properties[propsIndex]] = parsed
                propsIndex += 1
    return results

def generateHeatmap(results: List[Dict[str, List[int]]], prop: str) -> None:
    data = np.array([result[prop] for result in results]).T
    g = sns.heatmap(
        data=data
    )
    g.invert_yaxis()
    plt.xlabel("Epoch (approx 10s per increment)")
    plt.ylabel("Page Buckets (Bucket size 1027)")
    plt.title(f"Pagemap Metic: {prop}")
    plt.show()

def main():
    properties = [
        "active",
        "mincore",
        "referenced"
    ]
    results = parseSnapshots("snapshots", properties, 1007)
    generateHeatmap(results, "referenced")

if __name__ == "__main__":
    main()
#%%
