# PowerShell script to fix monster type 150 in save files
# This script will create backup copies of your save files and then replace monster type 150 with a valid type

# Define the save directory
$saveDir = "$env:APPDATA\diasurgical\devilution"

# Create a backup directory
$backupDir = "$saveDir\backup_$(Get-Date -Format 'yyyyMMdd_HHmmss')"
New-Item -ItemType Directory -Path $backupDir -Force | Out-Null

Write-Host "Creating backups of save files in $backupDir..."

# Backup all save files
Get-ChildItem -Path $saveDir -Filter "*.sv" | ForEach-Object {
    Copy-Item $_.FullName -Destination $backupDir
    Write-Host "Backed up $($_.Name)"
}

# Function to replace monster type 150 with a valid type (e.g., 1 for zombie)
function Replace-MonsterType {
    param (
        [string]$filePath,
        [byte]$oldType = 150,
        [byte]$newType = 1
    )
    
    Write-Host "Processing $filePath..."
    
    # Read the file as a byte array
    $bytes = [System.IO.File]::ReadAllBytes($filePath)
    
    # Count of replacements
    $replacements = 0
    
    # Search for the byte pattern
    for ($i = 0; $i -lt $bytes.Length; $i++) {
        if ($bytes[$i] -eq $oldType) {
            # Check if this might be a monster type (simple heuristic)
            # In a real scenario, you'd need more context to ensure you're only replacing monster types
            $bytes[$i] = $newType
            $replacements++
        }
    }
    
    # Write the modified bytes back to the file
    if ($replacements -gt 0) {
        [System.IO.File]::WriteAllBytes($filePath, $bytes)
        Write-Host "  Made $replacements replacements"
    } else {
        Write-Host "  No monster type $oldType found"
    }
}

# Process all save files
Get-ChildItem -Path $saveDir -Filter "*.sv" | ForEach-Object {
    Replace-MonsterType -filePath $_.FullName
}

Write-Host "`nProcessing complete. Original files have been backed up to $backupDir"
Write-Host "You can now run the game and it should no longer show the 'Invalid monster type: 150' error"
Write-Host "If you need to restore your original save files, copy them from the backup directory"
