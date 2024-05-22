import os
import pandas as pd
import torch
from transformers import RobertaTokenizer, RobertaForSequenceClassification

# Directory containing the sliced C++ files
input_directory = 'sliced_files'

# Load CodeBERT model and tokenizer
tokenizer = RobertaTokenizer.from_pretrained("microsoft/codebert-base")
model = RobertaForSequenceClassification.from_pretrained("microsoft/codebert-base")

# Function to predict SDC rate using CodeBERT
def predict_sdc_rate(source_code):
    inputs = tokenizer(source_code, return_tensors="pt", truncation=True, padding=True, max_length=512)
    with torch.no_grad():
        outputs = model(**inputs)
    probabilities = torch.nn.functional.softmax(outputs.logits, dim=-1)
    sdc_rate = probabilities[0][1].item()  # Assuming label 1 corresponds to SDC rate
    return sdc_rate

# Initialize an empty list to store all predictions
all_predictions = []

# Iterate over all sliced C++ files in the input directory
for root, dirs, files in os.walk(input_directory):
    for file in files:
        if file.endswith('.cpp'):
            file_path = os.path.join(root, file)
            with open(file_path, 'r') as f:
                source_code = f.read()
            sdc_rate = predict_sdc_rate(source_code)
            all_predictions.append((file, sdc_rate))

# Save predictions to a CSV file
df = pd.DataFrame(all_predictions, columns=['fileName', 'SDC_rate'])
df.to_csv('sdc_predictions.csv', index=False)
print('Predictions saved to sdc_predictions.csv')
