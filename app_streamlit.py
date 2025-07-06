
import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Title
st.title("Urban Air Quality and Health Impact Analysis")

# Upload data
st.sidebar.header("Upload Your CSV Data")
uploaded_file = st.sidebar.file_uploader("Choose a CSV file", type="csv")

if uploaded_file is not None:
    df = pd.read_csv(uploaded_file)
    st.write("### Data Preview", df.head())

    # Basic Statistics
    st.subheader("Summary Statistics")
    st.write(df.describe())

    # Plot AQI over time
    if 'Date' in df.columns and 'AQI' in df.columns:
        df['Date'] = pd.to_datetime(df['Date'])
        st.subheader("AQI Over Time")
        fig, ax = plt.subplots()
        df.plot(x='Date', y='AQI', ax=ax, color='red')
        st.pyplot(fig)

    # Correlation heatmap
    st.subheader("Correlation Heatmap")
    fig, ax = plt.subplots()
    sns.heatmap(df.corr(numeric_only=True), annot=True, cmap="coolwarm", ax=ax)
    st.pyplot(fig)
else:
    st.info("Awaiting CSV file to be uploaded.")
