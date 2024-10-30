import torch
import torch.nn as nn

# Define the CNN model for cloud classification
class WeatherModel(nn.Module):
  def __init__(self, num_classes):
    super(WeatherModel, self).__init__()
    self.conv1 = nn.Conv2d(3, 32, kernel_size=3, padding=1)
    self.pool = nn.MaxPool2d(2, 2)
    self.conv2 = nn.Conv2d(32, 64, kernel_size=3, padding=1)
    self.conv3 = nn.Conv2d(64, 128, kernel_size=3, padding=1)
    self.conv4 = nn.Conv2d(128, 128, kernel_size=3, padding=1)

    self.fc1 = nn.Linear(128 * 8 * 8, 256)
    self.dropout = nn.Dropout(0.5)
    self.fc2 = nn.Linear(256, num_classes)

  def forward(self, x):
    x = self.pool(torch.relu(self.conv1(x)))
    x = self.pool(torch.relu(self.conv2(x)))
    x = self.pool(torch.relu(self.conv3(x)))
    x = self.pool(torch.relu(self.conv4(x)))

    x = torch.flatten(x, 1)
    x = torch.relu(self.fc1(x))
    x = self.dropout(x)
    x = self.fc2(x)

    return x