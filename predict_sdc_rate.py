import os
import pandas as pd
import torch
from transformers import RobertaTokenizer, RobertaForSequenceClassification, RobertaConfig
from transformers import BertTokenizer, BertModel, AdamW
from torch import nn
from keybert import KeyBERT
import torch.nn.functional as F
# Directory containing the sliced C++ files
input_directory = 'sliced_files'

# Load CodeBERT model and tokenizer
model_path = "../CODEBERT-REGRESSION/code/BERTRegression/checkpoint-best-acc/model.bin"
tokenizer = RobertaTokenizer.from_pretrained("microsoft/codebert-base")
# Define a regression model on BERT
class BertRegressor(nn.Module):
    def __init__(self, bert_model="neulab/codebert-cpp", lstm_hidden_size=512, output_size=1):
        super().__init__()
        self.bert = BertModel.from_pretrained(bert_model)
        self.lstm = nn.LSTM(self.bert.config.hidden_size, lstm_hidden_size, batch_first=True)
        self.regressor = nn.Linear(lstm_hidden_size, output_size)

    def forward(self, input_ids, attention_mask):
        # Flatten input for BERT processing
        batch_size, seq_len, chunk_size = input_ids.size()
        input_ids = input_ids.view(-1, chunk_size)
        attention_mask = attention_mask.view(-1, chunk_size)

        with torch.no_grad():
            bert_output = self.bert(input_ids, attention_mask=attention_mask)

        # Extract [CLS] embeddings
        cls_embeddings = bert_output.last_hidden_state[:, 0, :].view(batch_size, seq_len, -1)

        # LSTM processing
        _, (hidden, _) = self.lstm(cls_embeddings)
        # Convert PyTorch tensor to NumPy array for KeyBERT
        cls_embeddings_np = hidden.squeeze(0).detach().cpu().numpy()
        print (cls_embeddings_np)
        # Extract the key features
        kw_model = KeyBERT()
        keywords = kw_model.extract_keywords(cls_embeddings_np, keyphrase_ngram_range=(1, 1), stop_words='none', use_maxsum=True, nr_candidates=20, top_n=5)
        print(keywords)

        # Regression
        return F.sigmoid(self.regressor(hidden.squeeze(0)))

model = BertRegressor()
# Evaluation
model.load_state_dict(torch.load(model_path))
model.eval()
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
